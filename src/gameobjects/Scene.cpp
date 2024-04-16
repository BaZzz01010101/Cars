#include "core.h"
#include "Scene.h"
#include "Helpers.h"

namespace game
{
  Scene::Scene(const Config& config) :
    terrain(config),
    config(config)
  {
  }

  void Scene::init()
  {
    terrain.init();
  }

  void Scene::update(float dt)
  {
    terrain.traceCount = 0;

    thread_local int slowMoCounter = 0;
    const int slowMoCounterMax = 40;
    slowMoCounter = (slowMoCounter + 1) % slowMoCounterMax;

    if (!paused && (!slowMotion || slowMoCounter == 0))
      updateGameObjects(dt);
  }

  void Scene::updateGameObjects(float dt)
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.isAlive(i))
      {
        Car& car = cars[i];
        car.update(dt);
        updateFiring(i, dt);
      }

    for (int i = 0; i < projectiles.capacity(); i++)
      if (projectiles.isAlive(i))
      {
        Projectile& projectile = projectiles[i];
        projectile.update(dt);
        vec3 direction = projectile.position - projectile.lastPosition;
        float distance = direction.length();
        direction /= distance;
        vec3 hitPosition, normal;
        int hitCarIndex = -1;

        if (projectile.lifeTime < 0)
          projectiles.remove(i);
        else if (traceRay(projectile.position, direction, distance, projectile.ownerIndex, &hitPosition, &normal, nullptr, &hitCarIndex))
        {
          projectiles.remove(i);

          Config::Graphics::ExplosionParticles explosionConfig = projectile.type == Projectile::Bullet ?
            config.graphics.bulletExplosionParticles :
            config.graphics.shellExplosionParticles;

          createExplosion(explosionConfig, hitPosition);

          if (hitCarIndex >= 0)
          {
            Car& hitCar = cars[hitCarIndex];
            hitCar.velocity += (hitCar.position - hitPosition).normalized() * projectile.baseDamage * 0.01f;

            int newHealth = std::max(hitCar.health - projectile.baseDamage, 0);

            if (hitCar.health > 0 && newHealth == 0)
              createExplosion(config.graphics.carExplosionParticles, hitCar.position);

            hitCar.health = newHealth;
          }
        }
      }

    for (int i = 0; i < explosionParticles.capacity(); i++)
      if (explosionParticles.isAlive(i))
      {
        ExplosionParticle& particle = explosionParticles[i];
        particle.update(dt);

        if (particle.lifeTime < 0)
          explosionParticles.remove(i);
      }
  }

  void Scene::updateFiring(int carIndex, float dt)
  {
    Car& car = cars[carIndex];

    if (car.gunFiring)
    {
      if (car.timeToNextGunFire <= 0)
      {
        const Config::Physics::Turret& gunConfig = config.physics.gun;
        const Turret& gun = car.gun;

        float bulletOffsetfix = gunConfig.projectileSpeed * -car.timeToNextGunFire;
        vec3 bulletPosition = gun.barrelPosition() + gun.forward() * bulletOffsetfix;
        vec3 barrelOffset = 0.2f * gun.left();
        vec3 position1 = bulletPosition + barrelOffset;
        vec3 position2 = bulletPosition - barrelOffset;
        vec3 velocity = car.velocity + gun.forward() * gunConfig.projectileSpeed;

        projectiles.tryAdd(Projectile {
          .lastPosition = position1,
          .lastVelocity = velocity,
          .position = position1,
          .velocity = velocity,
          .gravity = config.physics.gravity,
          .lifeTime = gunConfig.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = carIndex,
          .baseDamage = gunConfig.baseDamage,
          .type = Projectile::Type::Bullet,
          });

        projectiles.tryAdd(Projectile {
          .lastPosition = position2,
          .lastVelocity = velocity,
          .position = position2,
          .velocity = velocity,
          .gravity = config.physics.gravity,
          .lifeTime = gunConfig.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = carIndex,
          .baseDamage = gunConfig.baseDamage,
          .type = Projectile::Type::Bullet,
          });

        car.timeToNextGunFire += gunConfig.fireInterval;
      }
    }
    else
      car.timeToNextGunFire = 0;

    if (car.cannonFiring)
    {
      if (car.timeToNextCannonFire <= 0)
      {
        const Config::Physics::Turret& cannonConfig = config.physics.cannon;
        const Turret& cannon = car.cannon;
        vec3 position = cannon.barrelPosition();
        vec3 velocity = car.velocity + cannon.forward() * cannonConfig.projectileSpeed;

        projectiles.tryAdd(Projectile {
          .lastPosition = position,
          .lastVelocity = velocity,
          .position = position,
          .velocity = velocity,
          .gravity = config.physics.gravity,
          .lifeTime = cannonConfig.projectileLifeTime,
          .size = 0.2f,
          .ownerIndex = carIndex,
          .baseDamage = cannonConfig.baseDamage,
          .type = Projectile::Type::Shell,
          });

        car.timeToNextCannonFire = cannonConfig.fireInterval;
      }
    }

    car.timeToNextCannonFire -= dt;
    car.timeToNextGunFire -= dt;
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

  bool Scene::traceRay(vec3 origin, vec3 directionNormalized, float distance, int excludePlayerIndex, vec3* hitPosition, vec3* hitNormal, float* hitDistance, int* hitCarIndex) const
  {
    vec3 closestsHitPosition = vec3::zero;
    vec3 closestsHitNormal = vec3::zero;
    float closestsHitDistance = FLT_MAX;

    vec3 currentHitPosition = vec3::zero;
    vec3 currentHitNormal = vec3::zero;
    float currentHitDistance = FLT_MAX;

    if(terrain.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentHitNormal, &currentHitDistance) && currentHitDistance < closestsHitDistance)
    {
      closestsHitPosition = currentHitPosition;
      closestsHitNormal = currentHitNormal;
      closestsHitDistance = currentHitDistance;
    }

    for(int i = 0; i < cars.capacity(); i++)
      if(cars.isAlive(i) && i != excludePlayerIndex)
      {
        const Car& car = cars[i];

        if(car.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentHitNormal, &currentHitDistance) && currentHitDistance < closestsHitDistance)
        {
          closestsHitPosition = currentHitPosition;
          closestsHitNormal = currentHitNormal;
          closestsHitDistance = currentHitDistance;

          if (hitCarIndex)
            *hitCarIndex = i;
        }
      }

    if (closestsHitDistance != FLT_MAX)
    {
      if(hitPosition)
        *hitPosition = closestsHitPosition;

      if(hitNormal)
        *hitNormal = closestsHitNormal;

      if(hitDistance)
        *hitDistance = closestsHitDistance;

      return true;
    }

    return false;
  }

  void Scene::regenerateTerrain(Terrain::Mode mode)
  {
    terrain.generate(mode);
    Car& player = cars[playerIndex];
    reset(player.position, player.rotation);
  }

  void Scene::reset(vec3 playerPosition, quat playerRotation)
  {
    float terrainY = terrain.getHeight(playerPosition.x, playerPosition.z);
    playerPosition.y = terrainY + 2;
    Car& player = cars[playerIndex];
    player.resetToPosition(playerPosition, playerRotation);
  }

  void Scene::updatePlayerControl(const PlayerControl& playerControl)
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.isAlive(i) && cars[i].guid == playerControl.guid)
      {
        cars[i].updateControl(playerControl);
        return;
      }

    int index = cars.tryAdd(playerControl.guid, config, *this);

    if (index >= 0)
    {
      Car& car = cars[index];
      car.updateControl(playerControl);
    }
  }

  void Scene::syncPlayerState(const PlayerState& playerState, float syncFactor)
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.isAlive(i) && cars[i].guid == playerState.guid)
      {
        cars[i].syncState(playerState, syncFactor);
        return;
      }

    int index = cars.tryAdd(playerState.guid, config, *this);

    if (index >= 0)
    {
      Car& car = cars[index];
      car.syncState(playerState, syncFactor);
    }
  }

  void Scene::getPlayerState(int index, PlayerState* playerState) const
  {
    if (!playerState)
      return;

    const Car& player = cars[index];

    *playerState = {
      .guid = player.guid,
      .position = player.position,
      .rotation = player.rotation,
      .velocity = player.velocity,
      .angularVelocity = player.angularVelocity,
      .gunYaw = player.gun.yaw,
      .gunPitch = player.gun.pitch,
      .cannonYaw = player.cannon.yaw,
      .cannonPitch = player.cannon.pitch,
    };
  }

}