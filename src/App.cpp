#include "pch.h"
#include "App.h"
#include "Helpers.h"
#include "Config.h"

namespace game
{
  // TODO: Consider saving only required config parameters in dependent types
  // Saving the reference to Config in every instance can lead to serious multithreading issues
  // if config will be changed from different thread

  App::App() :
    config(Config::DEFAULT),
    scene(config),
    hud(config)
  {
  }

  void App::initialize()
  {
    InitWindow(config.graphics.screen.width, config.graphics.screen.height, config.graphics.screen.title);
    SetTargetFPS(60);

    scene.init();
    hud.init();
  }

  void App::run()
  {
    while (!WindowShouldClose())
    {
      updatePlayerControl();
      updateShortcuts();

      float dt = GetFrameTime();

      dt = clamp(dt, 0.0f, 0.1f);

      if (dt > 0)
        update(dt);

      draw();
    }
  }

  void App::shutdown()
  {
    CloseWindow();
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

  void App::draw()
  {
    ClearBackground(BLACK);

    BeginDrawing();

    scene.draw();
    hud.draw(scene);

    drawDebug();

    EndDrawing();
  }

  void App::togglePaused()
  {
    paused = !paused;
    scene.paused = paused;
    hud.paused = paused;
  }

  void App::drawDebug()
  {
  }

  void App::updatePlayerControl()
  {
    PlayerControl playerControl = {
      .uid = 0,
      .steeringAxis = float(IsKeyDown(KEY_A) - IsKeyDown(KEY_D)),
      .accelerationAxis = float(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)),
      .thrustAxis = float(IsKeyDown(KEY_LEFT_SHIFT)),
      .target = scene.getCameraTarget(),
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
      scene.drawWires = !scene.drawWires;

    if (IsKeyPressed(KEY_R))
      player.rotation = player.rotation * quat::fromEuler(PI / 2, 0, 0);

    if (IsKeyPressed(KEY_ZERO))
      scene.regenerateTerrain(Terrain::Normal);

    if (IsKeyPressed(KEY_ONE))
      scene.regenerateTerrain(Terrain::Debug1);

    if (IsKeyPressed(KEY_TWO))
      scene.regenerateTerrain(Terrain::Debug2);

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
      scene.getPlayer().syncState({
        .uid = 0,
        .position = {0, 20, 0},
        .rotation = quat::identity,
        .velocity = {0, 20, 10},
        .angularVelocity = {0, 0, 20},
        .gunYaw = PI / 2,
        .gunPitch = 0.5,
        .cannonYaw = -PI / 2,
        .cannonPitch = -0.5,
        });
    }
  }

}