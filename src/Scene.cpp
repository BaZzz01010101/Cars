#include "pch.h"
#include "Scene.h"
#include <Helpers.h>

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

    //turretModel = LoadModel(carConfig.graphics.turretModelPath);
    //turretModelLoaded = true;


    terrain.generate2(config.graphics.resources.terrainTexturePath, Terrain::Mode::Normal);

    float h = terrain.getHeight2(0, 0);

    player.position = { 0, h + 0.5f * player.size.y, 0 };
    player.debugName = "PlayerCar";
    //player.rotation = { 0, 0, 0, 1 };
    //player.angularSpeed = { 0.1, 0.2, 0.4 };
    //player.speed = vec3::zero;

    player.init(config, carModel, wheelModel, turretModel);

    camera.position = player.position + vec3{ -4, 1, 0 };
    camera.target = player.position;
    camera.up = vec3{ 0, 1, 0 };
    camera.fovy = 90;
    camera.projection = CAMERA_PERSPECTIVE;
  }

  void Scene::update(float dt)
  {
    const float speed = 5 * dt;

    vec3 camera_movement{
      speed * (IsKeyDown(KEY_W) - IsKeyDown(KEY_S)),
      speed * (IsKeyDown(KEY_D) - IsKeyDown(KEY_A)),
      speed * (IsKeyDown(KEY_LEFT_SHIFT) - IsKeyDown(KEY_LEFT_CONTROL)),
    };

    vec3 player_movement{
      speed * (IsKeyDown(KEY_HOME) - IsKeyDown(KEY_END)),
      speed * (IsKeyDown(KEY_LEFT_SHIFT) - IsKeyDown(KEY_LEFT_CONTROL)),
      speed * (IsKeyDown(KEY_PAGE_DOWN) - IsKeyDown(KEY_DELETE)),
    };

    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    UpdateCameraPro(&camera, camera_movement, vec3::zero, 0);

    vec3 thrust{
      0,
      -1000 + 2000 * float(IsKeyDown(KEY_SPACE)),
      0,
    };

    vec3 moment{
      400 * float(IsKeyDown(KEY_HOME) - IsKeyDown(KEY_END)),
      0,
      0,
    };

    //player_thrust = Vector3RotateByQuaternion(player_thrust, player.frontLeftWheel.rotation);
    //player.frontLeftWheel.turnSpeed = 2 * float(IsKeyDown(KEY_DELETE) - IsKeyDown(KEY_PAGE_DOWN));
    //player.frontLeftWheel.externalForce = thrust;
    //player.frontLeftWheel.externalMoment = moment;
    //player.position += player_movement;
    //float terrainY = terrain.getHeight2(player.position.x, player.position.z);
    //player.position.y = terrainY + 0.5f * player.size.y;

    thread_local int slowMoCounter = 0;
    const int slowMoCounterMax = 10;
    slowMoCounter = (slowMoCounter + 1) % slowMoCounterMax;

    if (!paused && (!slowMotion || slowMoCounter == 0))
      player.update(dt, terrain);

    followPlayer(camera, player);
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

  void Scene::togglePause()
  {
    paused = !paused;
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
    playerPosition.y = terrainY + 0.5f * player.size.y;
    player.resetToPosition(playerPosition, playerRotation);
    camera.position = playerPosition + vec3{ -4, 1, 0 };
    camera.target = playerPosition;
    camera.up = { 0, 1, 0 };
  }

  vec3 lastNForce{};
  vec3 lastMoment{};
  vec3 lastForce{};
  vec3 lastFrictionForce{};
  vec3 lastContactSpotVelocityProjected{};
  vec3 lastVelocity{};

  void Scene::followPlayer(Camera& camera, Car& player)
  {
    vec3 toPlayer = player.position - camera.position;
    float range = toPlayer.length();

    vec3 position = range > 10 ? vec3(camera.position) + (toPlayer + vec3{ 0, 5, 0 }).normalized() * (range - 10) :
      range < 5 ? vec3(camera.position) - toPlayer.normalized() * (5 - range) :
      camera.position;

    position = moveTo(camera.position, position, (position - camera.position).length() * 0.1f);
    position.y = std::max(position.y, terrain.getHeight2(position.x, position.z) + 0.5f);

    vec3 target = player.position;
    target = moveTo(camera.target, target, (target - camera.target).length() * 0.1f);

    camera.position = position;
    camera.target = target;
  }

}