#include "pch.h"
#include "App.h"
#include "Helpers.h"

namespace game
{
  // TODO: Consider saving only required config parameters in dependent types
  // Saving the reference to Config in every instance can lead to serious multithreading issues
  // if config will be changed from different thread

  App::App() :
    config(Config::DEFAULT),
    camera(config),
    scene(config),
    hud(config),
    renderer(config, camera, scene, hud)
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
    while (!WindowShouldClose())
    {
      updatePlayerControl();
      updateShortcuts();

      float dt = GetFrameTime();

      dt = clamp(dt, EPSILON, 0.1f);

      update(dt);

      const Car& player = scene.getPlayer();
      camera.update(dt, scene.terrain, player.position);

      renderer.draw();
    }
  }

  void App::shutdown()
  {
    renderer.shutdown();
  }

  void App::update(float dt)
  {
    if (dt > config.physics.maxDt)
    {
      update(0.5f * dt);
      update(0.5f * dt);

      return;
    }

    scene.update(dt);
    hud.update();
  }

  void App::togglePaused()
  {
    paused = !paused;
    scene.paused = paused;
    hud.paused = paused;
  }

  void App::updatePlayerControl()
  {
    PlayerControl playerControl = {
      .uid = 0,
      .steeringAxis = float(IsKeyDown(KEY_A) - IsKeyDown(KEY_D)),
      .accelerationAxis = float(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)),
      .thrustAxis = float(IsKeyDown(KEY_LEFT_SHIFT)),
      .target = camera.getTarget(),
      .primaryFire = IsMouseButtonDown(MOUSE_LEFT_BUTTON),
      .secondaryFire = IsMouseButtonDown(MOUSE_RIGHT_BUTTON),
      .handBrake = IsKeyDown(KEY_SPACE),
    };

    scene.updateLocalPlayerControl(playerControl);
  }

  void App::updateShortcuts()
  {
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
        .uid = 0,
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

      scene.getPlayer().syncState(playerState);
    }
  }

}