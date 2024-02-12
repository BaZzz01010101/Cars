#include "pch.h"
#include "App.h"
#include "Helpers.h"
#include "Config.h"

namespace game
{

  void App::initialize()
  {
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
        },
        .gun = {
          .minPitch = -PI / 16,
          .maxPitch = PI / 4,
          .minYaw = -PI * 3 / 4,
          .maxYaw = PI * 3 / 4,
          .rotationSpeed = PI / 2,
          .bodyFriction = 0.1f,
        },
        .cannon = {
          .minPitch = -PI / 16,
          .maxPitch = PI / 4,
          .minYaw = -PI / 4,
          .maxYaw = PI / 4,
          .rotationSpeed = PI / 2,
          .bodyFriction = 0.1f,
        }
      },
      .graphics = {
        .screen = {
          .title = "Mad Max",
          .width = 1280,
          .height = 720,
        },
        .camera = {
          .focusElevation = 5,
          .minDistance = 10,
          .maxDistance = 20,
          .maxPitch = PI * 3 / 8,
          .horzSensitivity = 1.0f,
          .vertSensitivity = 1.0f,
          .invertY = false,
        },
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
          .crosshairsTexturePath = "resources/textures/crosshairs.png",
        },
      }
    };

    InitWindow(config.graphics.screen.width, config.graphics.screen.height, config.graphics.screen.title);

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