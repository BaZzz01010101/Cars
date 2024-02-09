#include "pch.h"
#include "App.h"
#include "Helpers.h"
#include "Config.h"

namespace game
{

  void App::initialize()
  {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);

    config = {
      .physics = {
        .maxDt = 0.02f,
        .gravity = 9.8f,
        .car = {
          .mass = 1500,
          .enginePower = 350,
          .brakePower = 10000,
          .handBrakePower = 100000,
          .maxSpeed = 50,
          .minSteeringAngle = PI / 8,
          .maxSteeringAngle = PI / 4,
          .maxSteeringSpeed = PI / 2,
          .carAligningForce = 15,
          .bodyFriction = 0.05f,
          .aerodynamicKoef = 5.7f,
        },
        .turret = {
          .minPitch = -PI / 16,
          .maxPitch = PI / 4,
          .yawSpeed = PI / 2,
          .pitchSpeed = PI / 2,
          .bodyFriction = 0.1f,
        },
        .frontWheels = {
          .mass = 20,
          .radius = 0.5f,
          .suspensionStiffness = 100000,
          .suspensionDamping = 0.2f,
          .maxSuspensionOffset = 0.5f,
          .tireFriction = 1.0f,
          .rollingFriction = 0.01f,
        },
        .rearWheels = {
          .mass = 20,
          .radius = 0.5f,
          .suspensionStiffness = 200000,
          .suspensionDamping = 0.2f,
          .maxSuspensionOffset = 0.25f,
          .tireFriction = 1.0f,
          .rollingFriction = 0.01f,
        }
      },
      .graphics = {
        .hud = {
          .fontSize = 20,
          .screenMargins = 10,
        },
        .resources = {
          .fontPath = "resources/fonts/JetBrainsMono-Bold.ttf",
          .terrainTexturePath = "resources/textures/terrain.png",
          .carModelPath = "resources/models/jeep.gltf",
          .wheelModelPath = "resources/models/wheel.gltf",
          .turretModelPath = "resources/models/turret.gltf",
        },
      }
    };

    scene.init(config);
    hud.init(config);
  }

  void App::run()
  {
    while (!WindowShouldClose())
    {
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
    HideCursor();
    SetMousePosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
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
    scene.setPaused(paused);
    hud.setPaused(paused);
  }

  void App::drawDebug()
  {}

  void App::updateShortcuts()
  {
    if (IsKeyPressed(KEY_P))
      togglePaused();

    if (IsKeyPressed(KEY_O))
      scene.toggleSlowMotion();

    if (IsKeyPressed(KEY_T))
      scene.toggleDrawWires();

    if (IsKeyPressed(KEY_R))
      scene.player.rotation = scene.player.rotation * quat::fromEuler(PI / 2, 0, 0);

    if (IsKeyPressed(KEY_C))
      scene.toggleFirstPersonMode();

    if (IsKeyPressed(KEY_ZERO))
      scene.regenerateTerrain(config.graphics.resources.terrainTexturePath, Terrain::Normal);

    if (IsKeyPressed(KEY_ONE))
      scene.regenerateTerrain(config.graphics.resources.terrainTexturePath, Terrain::Debug1);

    if (IsKeyPressed(KEY_TWO))
      scene.regenerateTerrain(config.graphics.resources.terrainTexturePath, Terrain::Debug2);

    if (IsKeyPressed(KEY_F1))
      scene.reset(vec3::zero, quat::identity);

    if (IsKeyPressed(KEY_F2))
    {
      scene.reset({ 0, 0, 25 }, quat::identity);
      scene.player.rotation = quat::fromEuler(0, 0, 0.19f * PI);
    }

    if (IsKeyPressed(KEY_F3))
    {
      scene.reset({ 0, 0, 25 }, quat::identity);
      scene.player.rotation = quat::fromEuler(0, 0, 0.18f * PI);
      scene.player.rotation = scene.player.rotation * quat::fromEuler(PI / 2, 0, 0);
      scene.player.rotation = scene.player.rotation * quat::fromEuler(0, 0, 0.02f * PI);
    }
  }

}