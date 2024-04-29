#include "core.h"
#include "ClientApp.h"
#include "Helpers.h"

namespace game
{
  // TODO: Consider saving only required config parameters in dependent types
  // Saving the reference to Config in every instance can lead to serious multithreading issues
  // if config will be changed from different thread

  ClientApp::ClientApp(const Config& config, const ServerConfig& serverConfig) :
    exit(false),
    config(config),
    camera(config),
    scene(config),
    hud(config),
    renderer(config, camera, scene, hud),
    network(serverConfig, *this)
  {
  }

  void ClientApp::initialize()
  {
    renderer.init();
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

    if (scene.playerIndex < 0)
    {
      network.update();
      std::this_thread::sleep_for(milliseconds(1));

      // Fix bug with excessive high 1st rendered frame time and excessive physical frame counter promotion
      renderer.draw(0);

      return !exit && !WindowShouldClose();
    }

    updateShortcuts();

    PlayerControl localPlayerControl = getLocalPlayerControl();
    sendPlayerControl(localPlayerControl);
    scene.updatePlayerControl(localPlayerControl);

    network.update();

    float dt = GetFrameTime();
    dtAccumulator += dt;
    static const float fixedDt = config.physics.fixedDt;

    while (dtAccumulator > fixedDt)
    {
      // TODO: consider possible optimizations in physics calculations
      // by getting rid of 'dt' parameter in 'update' methods and by using fixed 'dt' from config
      update(fixedDt);
      dtAccumulator -= fixedDt;
    }

    float lerpFactor = dtAccumulator / fixedDt;
    updateCamera(dt, lerpFactor);
    renderer.draw(lerpFactor);

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

  PlayerControl ClientApp::getLocalPlayerControl()
  {
    const Car& player = scene.getPlayer();

    // TODO: Move the logic of disabled control for dead player to server side
    if (player.health == 0)
      return PlayerControl {
        .physicalFrame = scene.physicalFrame,
        .guid = player.guid,
        .steeringAxis = 0.0f,
        .accelerationAxis = 0.0f,
        .thrustAxis = 0.0f,
        .target = camera.getTarget(),
        .primaryFire = false,
        .secondaryFire = false,
        .handBrake = true,
    };

    return PlayerControl {
      .physicalFrame = scene.physicalFrame,
      .guid = player.guid,
      .steeringAxis = float(IsKeyDown(KEY_A) - IsKeyDown(KEY_D)),
      .accelerationAxis = float(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)),
      .thrustAxis = float(IsKeyDown(KEY_LEFT_SHIFT)),
      .target = camera.getTarget(),
      .primaryFire = IsMouseButtonDown(MOUSE_LEFT_BUTTON),
      .secondaryFire = IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
      .handBrake = IsKeyDown(KEY_SPACE),
    };
  }

  void ClientApp::onConnected(uint64_t guid)
  {
    printf("CLIENT_APP: OnConnected, my guid: %" PRIu64 "\n", guid);
    scene.playerGuid = guid;
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
    car.position = playerJoin.position;
    car.rotation = playerJoin.rotation;

    if (playerJoin.guid == scene.playerGuid)
    {
      scene.physicalFrame = playerJoin.physicalFrame;
      scene.playerIndex = index;
    }
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
  }

  void ClientApp::onPlayerControl(const PlayerControl& playerControl)
  {
    //if (playerControl.guid != scene.getPlayer().guid)
    scene.updatePlayerControl(playerControl);
    scene.serverPhysicalFrame = playerControl.physicalFrame;
  }

  void ClientApp::onPlayerState(const PlayerState& playerState)
  {
    scene.syncPlayerState(playerState, SYNC_FACTOR);
    scene.serverPhysicalFrame = playerState.physicalFrame;
  }

  void ClientApp::updateShortcuts()
  {
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_SPACE))
    {
      scene.cars.clear();
      network.disconnect();
      network.connect();
    }

    if (IsKeyPressed(KEY_P))
      togglePaused();

    if (IsKeyPressed(KEY_Y))
      camera.invertY = !camera.invertY;

    if (IsKeyPressed(KEY_O))
      scene.slowMotion = !scene.slowMotion;

    if (IsKeyPressed(KEY_T))
      renderer.drawWires = !renderer.drawWires;

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
      scene.reset(vec3::zero, quat::identity);

    if (scene.playerIndex < 0)
      return;

    Car& player = scene.getPlayer();

    if (IsKeyPressed(KEY_R))
      player.rotation = player.rotation * quat::fromEuler(PI / 2, 0, 0);

    if (IsKeyPressed(KEY_F2))
    {
      scene.reset({ 0, 0, 25 }, quat::identity);
      scene.getPlayer().rotation = quat::fromEuler(0, 0, 0.19f * PI);
    }

    if (IsKeyPressed(KEY_F3))
    {
      scene.reset({ 0, 0, 25 }, quat::identity);
      player.rotation = quat::fromEuler(0, 0, 0.18f * PI);
      player.rotation = player.rotation * quat::fromEuler(PI / 2, 0, 0);
      player.rotation = player.rotation * quat::fromEuler(0, 0, 0.02f * PI);
    }

    if (IsKeyPressed(KEY_F4))
    {
      PlayerState playerState = {
        .physicalFrame = scene.physicalFrame,
        .guid = scene.getPlayer().guid,
        .position = {-1, 7, 0},
        .rotation = quat::identity.rotatedByYAngle(PI / 2),
        .velocity = {randf(20, 50) * sign(randf(-1, 1)), 0, randf(20, 50) * sign(randf(-1, 1))},
        .angularVelocity = {0, 0, 0},
        .gunYaw = PI / 2,
        .gunPitch = 0.5,
        .cannonYaw = -PI / 2,
        .cannonPitch = -0.5,
      };

      playerState.position.y = 2 + scene.terrain.getHeight(playerState.position.x, playerState.position.z, nullptr);

      player.syncState(playerState, 1.0f);
    }
  }

  void ClientApp::sendPlayerControl(const PlayerControl& playerControl)
  {
    BitStream stream;
    playerControl.writeTo(stream);
    network.send(stream);
  }

  void ClientApp::updateCamera(float dt, float lerpFactor)
  {
    const Car& player = scene.getPlayer();
    vec3 playerPosition = vec3::lerp(player.lastPosition, player.position, lerpFactor);
    camera.update(dt, scene.terrain, playerPosition);
  }

}