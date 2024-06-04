#include "core.h"
#include "ClientApp.h"
#include "Helpers.h"

namespace game
{
  // TODO: Consider saving only required config parameters in dependent types
  // Saving the reference to Config in every instance can lead to serious multithreading issues
  // if config will be changed from different thread

  ClientApp::ClientApp(const Config& config, const WindowConfig& windowConfig, const ServerConfig& serverConfig) :
    exit(false),
    config(config),
    windowConfig(windowConfig),
    camera(config),
    scene(config, false),
    hud(config, matchStats),
    renderer(config, camera, scene, hud),
    network(serverConfig, *this)
  {
  }

  void ClientApp::initialize()
  {
    renderer.init(windowConfig);
    scene.init();
    hud.init();
    renderer.updateTerrainModel();
    dtAccumulator = 0;
    paused = false;
  }

  bool ClientApp::pulse()
  {
    if (!network.isConnected() && !network.connect())
      return false;

    if (scene.localPlayerIndex < 0)
    {
      network.update();
      std::this_thread::sleep_for(milliseconds(1));

      // Fix bug with excessive high 1st rendered frame time and excessive physical frame counter promotion
      renderer.draw(0);

      return !exit && !WindowShouldClose();
    }

    sendLocalPlayerControl();

    network.update();

    float dt = GetFrameTime();
    dtAccumulator += dt;
    static const float fixedDt = config.physics.fixedDt;

    if (dt > fixedDt * 5)
    {
      printf("WARNING!!! High 'dt' detected: %i ms\n", int(dt * 1000));
    }

    while (dtAccumulator > fixedDt)
    {
      // TODO: consider possible optimizations in physics calculations
      // by getting rid of 'dt' parameter in 'update' methods and by using fixed 'dt' from config
      update(fixedDt);
      dtAccumulator -= fixedDt;
    }

    float lerpFactor = scene.matchState == Scene::Scoreboard ? 1 : dtAccumulator / fixedDt;
    updateCamera(dt, lerpFactor);
    renderer.draw(lerpFactor);

    updateShortcuts();

    return !exit && !WindowShouldClose();
  }

  void ClientApp::shutdown()
  {
    renderer.shutdown();
    network.disconnect();
  }

  void ClientApp::update(float dt)
  {
    if (dt > config.physics.fixedDt)
    {
      update(0.5f * dt);
      update(0.5f * dt);

      return;
    }

    scene.update(dt);
    hud.update();
  }

  void ClientApp::togglePaused()
  {
    paused = !paused;
    scene.paused = paused;
    hud.paused = paused;
  }

  void ClientApp::onConnected(uint64_t guid)
  {
    printf("CLIENT_APP: OnConnected, my guid: %" PRIu64 "\n", guid);
    scene.localPlayerGuid = guid;
    scene.reset();
    matchStats.clear();
  }

  void ClientApp::onDisconnected(uint64_t guid)
  {
    printf("CLIENT_APP: OnDisconnected\n");
    exit = true;
  }

  void ClientApp::onPlayerJoin(const PlayerJoin& playerJoin)
  {
    printf("CLIENT_APP: OnPlayerJoin: %" PRIu64 "\n", playerJoin.guid);

    int index = scene.cars.tryAdd(playerJoin.guid, config, scene);

    if (index < 0)
    {
      printf("CLIENT_APP: Disconnecting! Players pool overflow.\n");
      network.disconnect();
      return;
    }

    Car& car = scene.cars[index];
    car.name = playerJoin.name;
    car.resetToPosition(playerJoin.position, playerJoin.rotation);
    car.switchToAliveState(Car::Countdown);

    if (playerJoin.guid == scene.localPlayerGuid)
    {
      scene.localPhysicalFrame = playerJoin.physicalFrame;
      scene.localPlayerIndex = index;
    }

    matchStats.addPlayer(playerJoin.guid, playerJoin.kills, playerJoin.deaths);
  }

  void ClientApp::onPlayerLeave(const PlayerLeave& playerLeave)
  {
    printf("CLIENT_APP: OnPlayerLeave: %" PRIu64 "\n", playerLeave.guid);

    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i) && scene.cars[i].guid == playerLeave.guid)
      {
        scene.cars.remove(i);
        break;
      }

    matchStats.removePlayer(playerLeave.guid);
  }

  void ClientApp::onPlayerControl(const PlayerControl& playerControl)
  {
    scene.updatePlayerControl(playerControl);
    scene.serverPhysicalFrame = playerControl.physicalFrame;
  }

  void ClientApp::onPlayerState(const PlayerState& playerState)
  {
    scene.syncPlayerState(playerState, SYNC_FACTOR);
    scene.serverPhysicalFrame = playerState.physicalFrame;
    matchStats.setPing(playerState.guid, playerState.ping);
  }

  void ClientApp::onPlayerHit(const PlayerHit& playerHit)
  {
    printf("CLIENT_APP: OnPlayerHit: %" PRIu64 " < %" PRIu64 "\n", playerHit.guid, playerHit.attakerGuid);
    // TODO: Implement player hit effect
    //scene.syncPlayerHit(playerHit);
  }

  void ClientApp::onPlayerKill(const PlayerKill& playerKill)
  {
    printf("CLIENT_APP: OnPlayerKill: %" PRIu64 " < %" PRIu64 "\n", playerKill.guid, playerKill.killerGuid);

    if (Car* killedPlayer = scene.tryGetPlayer(playerKill.guid))
    {
      scene.createExplosion(config.graphics.carExplosionParticles, killedPlayer->position);
      killedPlayer->health = 0;
      killedPlayer->switchToAliveState(Car::Dead);
      matchStats.addKill(playerKill.killerGuid);
      matchStats.addDeath(playerKill.guid);
    }
  }

  void ClientApp::onMatchState(const MatchState& matchState)
  {
    printf("CLIENT_APP: OnMatchState\n");

    scene.matchTimeout = matchState.matchTimeout;
    Scene::MatchState lastMatchState = scene.matchState;
    scene.updateMatchState();
    
    if(matchState.shouldResetMatchStats)
    {
      scene.reset();
      matchStats.reset();
    }
  }

  void ClientApp::updateShortcuts()
  {
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_SPACE))
    {
      scene.localPlayerIndex = -1;
      scene.cars.clear();
      network.disconnect();
      network.connect();
    }

    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
      switch (camera.mode)
      {
        case CustomCamera::Normal:
          camera.mode = CustomCamera::Zoom;
          break;

        case CustomCamera::Zoom:
          camera.mode = CustomCamera::Normal;
          break;

        default:
          break;
      }

    if (IsKeyPressed(KEY_P))
      togglePaused();

    if (IsKeyPressed(KEY_Y))
      camera.invertY = !camera.invertY;

    if (IsKeyPressed(KEY_O))
      scene.slowMotion = !scene.slowMotion;

    if (IsKeyPressed(KEY_T))
      renderer.drawWires = !renderer.drawWires;

    if (IsKeyPressed(KEY_D) && IsKeyDown(KEY_LEFT_ALT))
    {
      drawDebugInfo = !drawDebugInfo;
      renderer.drawDebugInfo = drawDebugInfo;
      hud.drawDebugInfo = drawDebugInfo;
    }

    if (IsKeyPressed(KEY_ZERO))
    {
      scene.regenerateTerrain(Terrain::Normal);
      renderer.updateTerrainModel();
    }

    if (IsKeyPressed(KEY_ONE))
    {
      scene.regenerateTerrain(Terrain::Debug1);
      renderer.updateTerrainModel();
    }

    if (IsKeyPressed(KEY_TWO))
    {
      scene.regenerateTerrain(Terrain::Debug2);
      renderer.updateTerrainModel();
    }

    if (IsKeyPressed(KEY_F1))
      scene.resetPlayer(vec3::zero, quat::identity);

    if (IsKeyPressed(KEY_TAB) || IsKeyPressedRepeat(KEY_TAB))
    {
      if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))
        hud.debugGraphs.selectPrev();
      else
        hud.debugGraphs.selectNext();
    }

    if (IsKeyPressed(KEY_EQUAL))
      hud.debugGraphs.addSelected();

    if (IsKeyPressed(KEY_MINUS))
      hud.debugGraphs.removeSelected();

    if (IsKeyPressed(KEY_BACKSPACE))
      hud.debugGraphs.removeAll();

    if (Car* player = scene.tryGetLocalPlayer())
    {
      if (IsKeyPressed(KEY_R))
        player->rotation = player->rotation * quat::fromEuler(PI / 2, 0, 0);

      if (IsKeyPressed(KEY_F2))
      {
        scene.resetPlayer({ 0, 0, 25 }, quat::identity);
        player->rotation = quat::fromEuler(0, 0, 0.19f * PI);
      }

      if (IsKeyPressed(KEY_F3))
      {
        scene.resetPlayer({ 0, 0, 25 }, quat::identity);
        player->rotation = quat::fromEuler(0, 0, 0.18f * PI);
        player->rotation = player->rotation * quat::fromEuler(PI / 2, 0, 0);
        player->rotation = player->rotation * quat::fromEuler(0, 0, 0.02f * PI);
      }

      if (IsKeyPressed(KEY_F4))
      {
        PlayerState playerState = {
          .physicalFrame = scene.localPhysicalFrame,
          .guid = scene.localPlayerGuid,
          .position = {-1, 7, 0},
          .rotation = quat::identity.rotatedByYAngle(PI / 2),
          .velocity = {randf(20, 50) * sign(randf(-1, 1)), 0, randf(20, 50) * sign(randf(-1, 1))},
          .angularVelocity = {0, 0, 0},
        };

        playerState.position.y = 2 + scene.terrain.getHeight(playerState.position.x, playerState.position.z, nullptr);

        player->syncState(playerState, 1.0f);
      }
    }
  }

  void ClientApp::sendLocalPlayerControl()
  {
    if (const Car* player = scene.tryGetLocalPlayer())
    {
      vec3 target = vec3::zero;
      float targetDistance = 100;

      // Fixes issue with crosshair twitching when targeting edge of terrain object at close/middle distance
      // In some conditions the same target point that hit by tracing from camera eye, did not hit when traced from turret barrel
      // So we penetrating the surface of the object by some distance to make sure that the target point is traceble from other direction
      const float targetPenetration = 0.5f;

      if (scene.traceRay(camera.position, camera.direction, FLT_MAX, scene.localPlayerGuid, nullptr, nullptr, &targetDistance, nullptr))
        targetDistance += targetPenetration;

      target = camera.position + camera.direction * targetDistance - player->position;

      PlayerControl playerControl = {
        .physicalFrame = scene.localPhysicalFrame,
        .guid = player->guid,
        .steeringAxis = float(IsKeyDown(KEY_A) - IsKeyDown(KEY_D)),
        .accelerationAxis = float(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)),
        .thrustAxis = float(IsKeyDown(KEY_LEFT_SHIFT)),
        .target = target,
        .primaryFire = IsMouseButtonDown(MOUSE_LEFT_BUTTON),
        .secondaryFire = IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
        .handBrake = IsKeyDown(KEY_SPACE),
      };

      BitStream stream;
      playerControl.writeTo(stream);
      network.send(stream);
    }
  }

  void ClientApp::updateCamera(float dt, float lerpFactor)
  {
    if (const Car* player = scene.tryGetLocalPlayer())
    {
      vec3 playerPosition = vec3::lerp(player->lastPosition, player->position, lerpFactor);
      camera.update(dt, scene.terrain, playerPosition);
    }
  }

}