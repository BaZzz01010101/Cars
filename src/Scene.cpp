#include "pch.h"
#include "Scene.h"
#include "Helpers.h"

namespace game
{
  Scene::Scene(const Config& config) :
    config(config),
    camera(config)
  {
  }

  Scene::~Scene()
  {
    void unloadResources();
  }

  void Scene::unloadResources()
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

  void Scene::init()
  {
    carModel = LoadModel(config.graphics.resources.carModelPath);
    carModelLoaded = true;

    wheelModel = LoadModel(config.graphics.resources.wheelModelPath);
    wheelModelLoaded = true;

    gunModel = LoadModel(config.graphics.resources.gunModelPath);
    gunModelLoaded = true;

    cannonModel = LoadModel(config.graphics.resources.cannonModelPath);
    cannonModelLoaded = true;

    terrain.generate(config.graphics.resources.terrainTexturePath, Terrain::Mode::Normal);

    playerIndex = cars.tryAdd(config, carModel, wheelModel, gunModel, cannonModel, terrain, camera);
    Car& player = cars.get(playerIndex);
    float h = terrain.getHeight(0, 0);
    player.position = { 0, h + 2, 0 };
  }

  void Scene::update(float dt)
  {
    updatePlayerControl(dt);
    updateFiring(dt);

    thread_local int slowMoCounter = 0;
    const int slowMoCounterMax = 40;
    slowMoCounter = (slowMoCounter + 1) % slowMoCounterMax;

    if (!paused && (!slowMotion || slowMoCounter == 0))
      updateGameObjects(dt);

    Car& player = cars.get(playerIndex);
    camera.update(dt, terrain, player.position);
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
        Projectile& projectile = projectiles.get(i);
        vec3 begin = projectile.position;
        projectile.update(dt);
        vec3 end = projectile.position;
        vec3 hitPosition, normal;

        if (terrain.traceRay(begin, (end - begin).normalized(), (end - begin).length(), &hitPosition, &normal))
        {
          projectiles.remove(i);

          Config::Graphics::ExplosionParticles explosionConfig = projectile.type == Projectile::Bullet ? 
            config.graphics.bulletExplosionParticles : 
            config.graphics.shellExplosionParticles;

          createExplosion(explosionConfig, hitPosition);
        }

        if (projectile.lifeTime < 0)
          projectiles.remove(i);
      }

    for (int i = 0; i < explosionParticles.size(); i++)
      if (explosionParticles.isAlive(i))
      {
        ExplosionParticle& particle = explosionParticles.get(i);
        particle.update(dt);

        if (particle.lifeTime < 0)
          explosionParticles.remove(i);
      }
  }

  void Scene::updateFiring(float dt)
  {
    if (gunFiring)
    {
      if (timeToNextGunFire <= 0)
      {
        const Config::Physics::Turret& gunConfig = config.physics.gun;
        const Car& player = cars.get(playerIndex);
        const Turret& gun = player.gun;

        float bulletOffsetfix = gunConfig.projectileSpeed * -timeToNextGunFire;
        vec3 bulletPosition = gun.barrelPosition() + gun.forward() * bulletOffsetfix;
        vec3 barrelOffset = 0.2f * gun.left();

        projectiles.tryAdd(Projectile{
          .position = bulletPosition + barrelOffset,
          .velocity = player.velocity + gun.forward() * gunConfig.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = gunConfig.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = playerIndex,
          .damage = gunConfig.baseDamage,
          .type = Projectile::Type::Bullet,
          });

        projectiles.tryAdd(Projectile{
          .position = bulletPosition - barrelOffset,
          .velocity = player.velocity + gun.forward() * gunConfig.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = gunConfig.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = playerIndex,
          .damage = gunConfig.baseDamage,
          .type = Projectile::Type::Bullet,
          });

        timeToNextGunFire = gunConfig.fireInterval;
      }
    }
    else
      timeToNextGunFire = 0;

    if (cannonFiring)
    {
      if (timeToNextCannonFire <= 0)
      {
        const Config::Physics::Turret& cannonConfig = config.physics.cannon;
        const Car& player = cars.get(playerIndex);
        const Turret& cannon = player.cannon;

        projectiles.tryAdd(Projectile{
          .position = cannon.barrelPosition(),
          .velocity = player.velocity + cannon.forward() * cannonConfig.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = cannonConfig.projectileLifeTime,
          .size = 0.2f,
          .ownerIndex = playerIndex,
          .damage = cannonConfig.baseDamage,
          .type = Projectile::Type::Shell,
          });

        timeToNextCannonFire = cannonConfig.fireInterval;
      }
    }

    timeToNextCannonFire -= dt;
    timeToNextGunFire -= dt;
  }

  void Scene::createExplosion(const Config::Graphics::ExplosionParticles& config, vec3 position)
  {
    for (int i = 0; i < config.count; i++)
      explosionParticles.tryAdd(ExplosionParticle::random(
        position,
        config.minSize,
        config.maxSize,
        config.minSpeed,
        config.maxSpeed,
        config.minAngularSpeed,
        config.maxAngularSpeed,
        config.minLifeTime,
        config.maxLifeTime,
        this->config.physics.gravity
      ));
  }

  void Scene::updatePlayerControl(float dt)
  {
    Car& player = getPlayer();
    const Config::Physics::Car& carConfig = config.physics.car;

    player.verticalTrust = player.mass * 20 * float(IsKeyDown(KEY_LEFT_SHIFT)),
    player.handBreaked = IsKeyDown(KEY_SPACE);
    player.enginePowerDirection = float(IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
    player.steeringDirection = float(IsKeyDown(KEY_A) - IsKeyDown(KEY_D));
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

    for (int i = 0; i < explosionParticles.size(); i++)
      if (explosionParticles.isAlive(i))
      {
        ExplosionParticle& particle = explosionParticles.get(i);
        particle.draw();
      }

    EndMode3D();
  }

  void Scene::regenerateTerrain(const char* texturePath, Terrain::Mode mode)
  {
    terrain.generate(texturePath, mode);
    Car& player = cars.get(playerIndex);
    reset(player.position, player.rotation);
  }

  void Scene::reset(vec3 playerPosition, quat playerRotation)
  {
    float terrainY = terrain.getHeight(playerPosition.x, playerPosition.z);
    playerPosition.y = terrainY + 2;
    Car& player = cars.get(playerIndex);
    player.resetToPosition(playerPosition, playerRotation);
    camera.reset(playerPosition);
  }

}