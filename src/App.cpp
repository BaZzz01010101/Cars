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
        .car = {
          .mass = 1000,
          .enginePower = 3000,
          .brakePower = 10000,
          .handBrakePower = 100000,
          .maxSpeed = 30,
          .maxSteeringAngle = PI / 4,
          .maxSteeringSpeed = PI / 2,
          .carAligningForce = 15,
          .bodyFriction = 0.05f,
        },
        .frontWheels = {
          .mass = 20,
          .radius = 0.5f,
          .suspensionStiffness = 200000,
          .suspensionDamping = 2000,
          .maxSuspensionOffset = 0.5f,
          .tireFriction = 0.8f,
          .rollingFriction = 0.01f,
        },
        .rearWheels = {
          .mass = 20,
          .radius = 0.5f,
          .suspensionStiffness = 200000,
          .suspensionDamping = 2000,
          .maxSuspensionOffset = 0.25f,
          .tireFriction = 0.8f,
          .rollingFriction = 0.01f,
        }
      },
      .graphics = {
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
  }

  void App::draw()
  {
    HideCursor();
    SetMousePosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    ClearBackground(BLACK);

    BeginDrawing();

    scene.draw();
    drawDebug();

    EndDrawing();
  }

  void App::drawDebug()
  {
    //if (player.isGrounded)
    //  DrawText("Grounded", 10, 40, 20, GREEN);

    //if (player.isSliding)
    //  DrawText("Sliding", 10, 60, 20, YELLOW);

    //DrawText(TextFormat("Wheel XYZ: %.3f, %.3f, %.3f", scene.player.position.x, scene.player.position.y, scene.player.position.z), 10, 80, 20, WHITE);

    DrawFPS(10, 10);
  }

  void App::updateShortcuts()
  {
    if (IsKeyPressed(KEY_SPACE))
      scene.togglePause();

    if (IsKeyPressed(KEY_O))
      scene.toggleSlowMotion();

    if (IsKeyPressed(KEY_T))
      scene.toggleDrawWires();

    if (IsKeyPressed(KEY_ZERO))
      scene.regenerateTerrain(config.graphics.resources.terrainTexturePath, Terrain::Normal);

    if (IsKeyPressed(KEY_ONE))
      scene.regenerateTerrain(config.graphics.resources.terrainTexturePath, Terrain::Debug1);

    if (IsKeyPressed(KEY_TWO))
      scene.regenerateTerrain(config.graphics.resources.terrainTexturePath, Terrain::Debug2);

    if (IsKeyPressed(KEY_F1))
      scene.reset(vec3::zero, quat::identity);

    if (IsKeyPressed(KEY_F2))
      scene.reset({ 0, 0, 25 }, quat::identity);
  }

}