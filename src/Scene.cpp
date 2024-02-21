#include "pch.h"
#include "Scene.h"
#include "Helpers.h"

namespace game
{
  Scene::Scene()
  {}

  Scene::~Scene()
  {
    if (carModelLoaded)
      UnloadModel(carModel);

    if (wheelModelLoaded)
      UnloadModel(wheelModel);

    if (gunModelLoaded)
      UnloadModel(gunModel);

    if (cannonModelLoaded)
      UnloadModel(cannonModel);
  }

  void Scene::init(const Config& config)
  {
    SetTargetFPS(60);

    this->config = config;

    carModel = LoadModel(config.graphics.resources.carModelPath);
    carModelLoaded = true;

    wheelModel = LoadModel(config.graphics.resources.wheelModelPath);
    wheelModelLoaded = true;

    gunModel = LoadModel(config.graphics.resources.gunModelPath);
    gunModelLoaded = true;

    cannonModel = LoadModel(config.graphics.resources.cannonModelPath);
    cannonModelLoaded = true;

    terrain.generate2(config.graphics.resources.terrainTexturePath, Terrain::Mode::Normal);

    playerIndex = cars.tryAdd();
    Car& player = cars.get(playerIndex);
    float h = terrain.getHeight2(0, 0);
    player.position = { 0, h + 2, 0 };
    player.init(config, carModel, wheelModel, gunModel, cannonModel, terrain, camera);

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
      updateGameObjects(dt);

    Car& player = cars.get(playerIndex);
    camera.update(dt, terrain, player.position);

    updateFiring(dt);
  }

  void Scene::updateGameObjects(float dt)
  {
    for (int i = 0; i < cars.size(); i++)
    {
      Car& car = cars.get(i);
      car.update(dt);
    }

    for (int i = 0; i < projectiles.size(); i++)
      if (projectiles.isAlive(i))
      {
        Projectile& bullet = projectiles.get(i);
        bullet.update(dt);

        if (bullet.lifeTime < 0)
          projectiles.remove(i);
      }
  }

  void Scene::updateFiring(float dt)
  {
    if (gunFiring)
    {
      if (timeToNextGunFire <= 0)
      {
        Car& player = cars.get(playerIndex);

        projectiles.tryAdd(Projectile{
          .position = player.gun.position + vec3::up - 0.25f * player.gun.left() + player.gun.forward() * (5.5f + config.physics.gun.projectileSpeed * -timeToNextGunFire),
          .velocity = player.velocity + player.gun.forward() * config.physics.gun.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = config.physics.gun.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = playerIndex,
          .damage = config.physics.gun.baseDamage,
          });

        projectiles.tryAdd(Projectile{
          .position = player.gun.position + vec3::up + 0.25f * player.gun.left() + player.gun.forward() * (5.5f + config.physics.gun.projectileSpeed * -timeToNextGunFire),
          .velocity = player.velocity + player.gun.forward() * config.physics.gun.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = config.physics.gun.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = playerIndex,
          .damage = config.physics.gun.baseDamage,
          });

        timeToNextGunFire = config.physics.gun.fireInterval;
      }
    }
    else
      timeToNextGunFire = 0;

    if (cannonFiring)
    {
      if (timeToNextCannonFire <= 0)
      {
        Car& player = cars.get(playerIndex);

        projectiles.tryAdd(Projectile{
          .position = player.cannon.position + vec3::up + 5 * player.cannon.forward(),
          .velocity = player.velocity + player.cannon.forward() * config.physics.cannon.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = config.physics.cannon.projectileLifeTime,
          .size = 0.2f,
          .ownerIndex = playerIndex,
          .damage = config.physics.cannon.baseDamage,
          });

        timeToNextCannonFire = config.physics.cannon.fireInterval;
      }
    }

    timeToNextCannonFire -= dt;
    timeToNextGunFire -= dt;
  }

  void Scene::draw()
  {
    BeginMode3D(camera);

    Car& player = cars.get(playerIndex);
    terrain.draw(drawWires);

    for (int i = 0; i < cars.size(); i++)
    {
      Car& car = cars.get(i);
      car.draw(drawWires);
    }

    for (int i = 0; i < projectiles.size(); i++)
      if (projectiles.isAlive(i))
      {
        Projectile& bullet = projectiles.get(i);
        bullet.draw();
      }

    EndMode3D();
  }

  void Scene::regenerateTerrain(const char* texturePath, Terrain::Mode mode)
  {
    terrain.generate2(texturePath, mode);
    Car& player = cars.get(playerIndex);
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
    Car& player = cars.get(playerIndex);
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