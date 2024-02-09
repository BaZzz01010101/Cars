#include "pch.h"
#include "Scene.h"
#include "Helpers.h"

namespace game
{
  Scene::~Scene()
  {
    if (carModelLoaded)
      UnloadModel(carModel);

    if (wheelModelLoaded)
      UnloadModel(wheelModel);

    if (turretModelLoaded)
      UnloadModel(turretModel);
  }

  void Scene::init(const Config& config)
  {
    SetTargetFPS(60);

    carModel = LoadModel(config.graphics.resources.carModelPath);
    carModelLoaded = true;

    wheelModel = LoadModel(config.graphics.resources.wheelModelPath);
    wheelModelLoaded = true;

    turretModel = LoadModel(config.graphics.resources.turretModelPath);
    turretModelLoaded = true;

    terrain.generate2(config.graphics.resources.terrainTexturePath, Terrain::Mode::Normal);

    float h = terrain.getHeight2(0, 0);
    player.position = { 0, h + 2, 0 };
    player.init(config, carModel, wheelModel, turretModel);

    camera.init(config.graphics);
  }

  void Scene::update(float dt)
  {
    const float speed = 5 * dt;

    vec3 camera_movement{
      speed * (IsKeyDown(KEY_HOME) - IsKeyDown(KEY_END)),
      speed * (IsKeyDown(KEY_LEFT_SHIFT) - IsKeyDown(KEY_LEFT_CONTROL)),
      speed * (IsKeyDown(KEY_PAGE_DOWN) - IsKeyDown(KEY_DELETE)),
    };

    vec3 player_movement{
      speed * (IsKeyDown(KEY_W) - IsKeyDown(KEY_S)),
      speed * (IsKeyDown(KEY_D) - IsKeyDown(KEY_A)),
      speed * (IsKeyDown(KEY_LEFT_SHIFT) - IsKeyDown(KEY_LEFT_CONTROL)),
    };

    thread_local int slowMoCounter = 0;
    const int slowMoCounterMax = 40;
    slowMoCounter = (slowMoCounter + 1) % slowMoCounterMax;

    if (!paused && (!slowMotion || slowMoCounter == 0))
      player.update(dt, terrain, camera);

    camera.update(dt, terrain, player.position);
  }

  void Scene::draw()
  {
    BeginMode3D(camera);

    terrain.draw(drawWires);
    player.draw(drawWires);

    EndMode3D();
  }

  void Scene::regenerateTerrain(const char* texturePath, Terrain::Mode mode)
  {
    terrain.generate2(texturePath, mode);
    reset(player.position, player.rotation);
  }

  void Scene::toggleDrawWires()
  {
    drawWires = !drawWires;
  }

  void Scene::toggleSlowMotion()
  {
    slowMotion = !slowMotion;
  }

  void Scene::reset(vec3 playerPosition, quat playerRotation)
  {
    float terrainY = terrain.getHeight2(playerPosition.x, playerPosition.z);
    playerPosition.y = terrainY + 2;
    player.resetToPosition(playerPosition, playerRotation);
    camera.reset(playerPosition);
  }

  vec3 lastNForce{};
  vec3 lastMoment{};
  vec3 lastForce{};
  vec3 lastFrictionForce{};
  vec3 lastContactSpotVelocityProjected{};
  vec3 lastVelocity{};

}