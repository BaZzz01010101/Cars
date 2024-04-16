#include "core.h"
#include "App.h"
#include "Helpers.h"

namespace game
{
  // TODO: Consider saving only required config parameters in dependent types
  // Saving the reference to Config in every instance can lead to serious multithreading issues
  // if config will be changed from different thread

  App::App() :
    exit(false),
    config(Config::DEFAULT),
    camera(config),
    scene(config),
    hud(config),
    renderer(config, camera, scene, hud),
    network(config, *this)
  {
  }

  void App::initialize()
  {
    renderer.init();
    scene.init();
    hud.init();
    renderer.updateTerrainModel();
  }

  void App::run()
  {
    if (!network.connect())
      return;

    float dtAccumulator = 0;
    float fixedDt = config.physics.fixedDt;

    while (!exit && !WindowShouldClose())
    {
      updateLocalPlayerControl();
      updateShortcuts();
      network.update();

      float dt = GetFrameTime();
      dtAccumulator += dt;

      while (dtAccumulator > fixedDt)
      {
        // TODO: consider possible optimizations in physics calculations
        // by getting rid of 'dt' parameter in 'update' methods and by using fixed 'dt' from config
        update(fixedDt);
        dtAccumulator -= fixedDt;
      }

      sendLocalPlayerState();

      float lerpFactor = dtAccumulator / fixedDt;
      updateCamera(dt, lerpFactor);
      renderer.draw(lerpFactor);
    }
  }

  void App::shutdown()
  {
    renderer.shutdown();
    network.disconnect();
  }

  void App::update(float dt)
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

  void App::updateLocalPlayerControl()
  {
    PlayerControl localPlayerControl = getLocalPlayerControl();
    scene.updatePlayerControl(localPlayerControl);

    BitStream localPlayerControlStream;
    localPlayerControl.writeTo(localPlayerControlStream);
    network.send(localPlayerControlStream);
  }

  void App::togglePaused()
  {
    paused = !paused;
    scene.paused = paused;
    hud.paused = paused;
  }

  PlayerControl App::getLocalPlayerControl()
  {
    return PlayerControl {
      .guid = scene.getPlayer().guid,
      .steeringAxis = float(IsKeyDown(KEY_A) - IsKeyDown(KEY_D)),
      .accelerationAxis = float(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)),
      .thrustAxis = float(IsKeyDown(KEY_LEFT_SHIFT)),
      .target = camera.getTarget(),
      .primaryFire = IsMouseButtonDown(MOUSE_LEFT_BUTTON),
      .secondaryFire = IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
      .handBrake = IsKeyDown(KEY_SPACE),
    };
  }

  void App::onConnected(uint64_t guid)
  {
    int playerIndex = scene.cars.tryAdd(guid, config, scene);

    if (playerIndex < 0)
    {
      printf("CLIENT_APP: OnConnected. Disconnecting! Players pool overflow.\n");
      network.disconnect();
      return;
    }

    printf("CLIENT_APP: OnConnected, my guid: %" PRIu64 "\n", guid);
    scene.playerIndex = playerIndex;
    Car& player = scene.getPlayer();
    float h = scene.terrain.getHeight(0, 0);
    player.position = { 0, h + 2, 0 };
  }

  void App::onDisconnected(uint64_t guid)
  {
    printf("CLIENT_APP: OnDisconnected\n");
    exit = true;
  }

  void App::onPlayerJoin(const PlayerJoin& playerJoin)
  {
    printf("CLIENT_APP: OnPlayerJoin: %" PRIu64 "\n", playerJoin.guid);

    if (playerJoin.guid != scene.getPlayer().guid)
    {
      int index = scene.cars.tryAdd(playerJoin.guid, config, scene);
      Car& car = scene.cars[index];
      car.position = playerJoin.position;
      car.rotation = playerJoin.rotation;
    }
    else
    {
      Car& car = scene.getPlayer();
      car.position = playerJoin.position;
      car.rotation = playerJoin.rotation;
    }
  }

  void App::onPlayerLeave(const PlayerLeave& playerLeave)
  {
    printf("CLIENT_APP: OnPlayerLeave: %" PRIu64 "\n", playerLeave.guid);

    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i) && scene.cars[i].guid == playerLeave.guid)
      {
        scene.cars.remove(i);
        break;
      }
  }

  void App::onPlayerControl(const PlayerControl& playerControl)
  {
    //if (playerControl.guid != scene.getPlayer().guid)
    scene.updatePlayerControl(playerControl);
  }

  void App::onPlayerState(const PlayerState& playerState)
  {
    if (playerState.guid != scene.getPlayer().guid)
      scene.syncPlayerState(playerState, SYNC_FACTOR);
  }

  void App::updateShortcuts()
  {
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_SPACE))
    {
      scene.cars.clear();
      network.disconnect();
      network.connect();
    }

    Car& player = scene.getPlayer();

    if (IsKeyPressed(KEY_P))
      togglePaused();

    if (IsKeyPressed(KEY_O))
      scene.slowMotion = !scene.slowMotion;

    if (IsKeyPressed(KEY_T))
      renderer.drawWires = !renderer.drawWires;

    if (IsKeyPressed(KEY_R))
      player.rotation = player.rotation * quat::fromEuler(PI / 2, 0, 0);

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

      scene.getPlayer().syncState(playerState, 1.0f);
    }
  }

  void App::sendLocalPlayerState()
  {
    PlayerState playerState;
    scene.getPlayerState(scene.playerIndex, &playerState);

    BitStream playerStateStream;
    playerState.writeTo(playerStateStream);

    network.send(playerStateStream);
  }

  void App::updateCamera(float dt, float lerpFactor)
  {
    const Car& player = scene.getPlayer();
    vec3 playerPosition = vec3::lerp(player.lastPosition, player.position, lerpFactor);
    camera.update(dt, scene.terrain, playerPosition);
  }

}