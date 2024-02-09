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
    const int slowMoCounterMax = 40;
    slowMoCounter = (slowMoCounter + 1) % slowMoCounterMax;

    if (!paused && (!slowMotion || slowMoCounter == 0))
      player.update(dt, terrain, camera.target);

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
    if (firstPersonMode)
    {
      vec3 offs = 1.2f * player.forward() + 1.1f * player.up();
      vec3 pos = player.position + offs - 0.2f * player.up();
      camera.position = pos;
      camera.target = pos + player.forward();
      camera.up = player.up();

      return;
    }

    vec3 toPlayer = player.position - camera.position;
    float range = toPlayer.length();

    float MIN_RANGE = 10;
    float MAX_RANGE = 20;

    vec3 position = range > MAX_RANGE ? vec3(camera.position) + (toPlayer + 5 * vec3::up).normalized() * (range - MAX_RANGE) :
      range < MIN_RANGE ? vec3(camera.position) - toPlayer.normalized() * (MIN_RANGE - range) :
      camera.position;

    vec3 sight = player.position;
    sight.y += 5;

    //position = moveTo(camera.position, position, (position - camera.position).length() * 0.1f);
    position.y = std::max(position.y, terrain.getHeight2(position.x, position.z) + 0.5f);
    //position.y = player.position.y + 10.0f;

    vec3 target = player.position + vec3::up * 8.0f;
    //target = moveTo(camera.target, target, (target - camera.target).length() * 0.1f);

    camera.position = position;
    camera.target = target;
    camera.up = vec3{ 0, 1, 0 };
  }

}