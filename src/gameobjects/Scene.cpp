#include "core.h"
#include "Scene.h"
#include "Helpers.h"

namespace game
{
  Scene::Scene(const Config& config, bool isServer) :
    isServer(isServer),
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
    localPhysicalFrame++;
    terrain.traceCount = 0;

    thread_local int slowMoCounter = 0;
    const int slowMoCounterMax = 40;
    slowMoCounter = (slowMoCounter + 1) % slowMoCounterMax;

    if (!paused && (!slowMotion || slowMoCounter == 0))
    {
      updateGameObjects(dt);

      if (isServer)
        updateRespawn();
    }
  }

  void Scene::updateGameObjects(float dt)
  {
    clearHits();
    clearKills();

    // TODO: Separate onto updateProjectiles, updateCars and updateParticles
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
        float hitDistance = 0;

        if (projectile.lifeTime < 0)
          projectiles.remove(i);
        else if (traceRay(projectile.lastPosition, direction, distance, projectile.ownerIndex, &hitPosition, &normal, &hitDistance, &hitCarIndex))
        {
          projectiles.remove(i);

          if (!isServer)
          {
            Config::Graphics::ExplosionParticles explosionConfig = projectile.type == Projectile::Bullet ?
              config.graphics.bulletExplosionParticles :
              config.graphics.shellExplosionParticles;

            createExplosion(explosionConfig, hitPosition);
          }

          if (isServer && hitCarIndex >= 0)
          {
            Car& hitCar = cars[hitCarIndex];
            hitCar.hitForce += direction * hitCar.mass * float(projectile.baseDamage) * 0.1f;
            hitCar.hitMoment += ((hitPosition - hitCar.position) % hitCar.hitForce).rotatedBy(hitCar.rotation.inverted());
            uint64_t hitTick = localPhysicalFrame * 100 + int(100 * hitDistance / distance);
            addHit(hitCarIndex, { hitTick, projectile.ownerIndex, projectile.baseDamage });
          }
        }
      }

    if (isServer)
    {
      applyHits();
      applyKills();
    }

    for (int i = 0; i < cars.capacity(); i++)
      if (cars.isAlive(i))
      {
        Car& car = cars[i];
        car.update(dt);
        updateFiring(i, dt);
      }

    if (!isServer)
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
      while (car.timeToNextGunFire <= 0)
      {
        const Config::Physics::Turret& gunConfig = config.physics.gun;
        const Turret& gun = car.gun;

        float bulletOffsetfix = gunConfig.projectileSpeed * -car.timeToNextGunFire;
        vec3 bulletPosition = gun.barrelFrontPosition() + gun.forward() * bulletOffsetfix;
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
        car.gunRecoilForce -= gun.forward() * car.mass * 30 * gunConfig.fireInterval;
      }
    }

    if (car.cannonFiring)
    {
      if (car.timeToNextCannonFire <= 0)
      {
        const Config::Physics::Turret& cannonConfig = config.physics.cannon;
        const Turret& cannon = car.cannon;
        vec3 position = cannon.barrelFrontPosition();
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
        car.cannonRecoilForce -= cannon.forward() * car.mass * 10;
      }
    }

    car.timeToNextCannonFire -= dt;
    car.timeToNextGunFire -= dt;

    if (!car.gunFiring && car.timeToNextGunFire < 0)
      car.timeToNextGunFire = 0;

    if (!car.cannonFiring && car.timeToNextCannonFire < 0)
      car.timeToNextCannonFire = 0;
  }

  void Scene::updateRespawn()
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.isAlive(i))
      {
        Car& car = cars[i];

        if (car.health <= 0 && car.deathTimeout <= 0)
        {
          car.health = config.physics.car.maxHealth;
          respawnCar(car);
        }
      }
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

    if (terrain.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentHitNormal, &currentHitDistance) && currentHitDistance < closestsHitDistance)
    {
      closestsHitPosition = currentHitPosition;
      closestsHitNormal = currentHitNormal;
      closestsHitDistance = currentHitDistance;
    }

    for (int i = 0; i < cars.capacity(); i++)
      if (cars.isAlive(i) && i != excludePlayerIndex)
      {
        const Car& car = cars[i];

        if (!car.isRespawning() && car.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentHitNormal, &currentHitDistance) && currentHitDistance < closestsHitDistance)
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
      if (hitPosition)
        *hitPosition = closestsHitPosition;

      if (hitNormal)
        *hitNormal = closestsHitNormal;

      if (hitDistance)
        *hitDistance = closestsHitDistance;

      return true;
    }

    return false;
  }

  void Scene::clearKills()
  {
    kills.clear();
  }

  void Scene::clearHits()
  {
    for (int i = 0; i < MAX_CARS; i++)
      hits[i].clear();
  }

  void Scene::addHit(int carIndex, Hit hit)
  {
    SemiVector<Hit, MAX_CARS>& carHits = hits[carIndex];

    if (carHits.isEmpty())
    {
      carHits.add(hit);

      return;
    }

    int lastIndex = carHits.size() - 1;

    if (hit.tick >= carHits[lastIndex].tick)
    {
      carHits.add(hit);

      return;
    }

    for (int i = 0; i < carHits.size(); i++)
      if (hit.tick < carHits[i].tick)
      {
        carHits.add(carHits[lastIndex]);

        for (int j = lastIndex; j > i; j--)
          carHits[j] = carHits[j - 1];

        carHits[i] = hit;

        break;
      }
  }

  void Scene::applyHits()
  {
    for (int i = 0; i < MAX_CARS; i++)
      if (cars.isAlive(i))
      {
        Car& player = cars[i];

        if (IsKeyPressed(KEY_F7))
          player.velocity = vec2::randomInRing(10, 10).intoXZWithY(randf(5, 10));

        if (player.health <= 0)
          continue;

        SemiVector<Hit, MAX_CARS>& carHits = hits[i];

        for (int j = 0; j < carHits.size(); j++)
        {
          Hit& hit = carHits[j];
          player.health -= hit.damage;

          if (player.health <= 0)
          {
            Car& attacker = cars[hit.attackerIndex];
            kills.add({ player.guid, attacker.guid });

            break;
          }
        }
      }
  }

  void Scene::applyKills()
  {
    for (int i = 0; i < kills.size(); i++)
    {
      Kill& kill = kills[i];

      if (Car* player = tryGetPlayer(kill.playerGuid))
      {
        player->resetDeathTimeouts();
        player->velocity = vec2::randomInRing(5, 5).intoXZWithY(randf(5, 10));
      }
    }
  }

  void Scene::respawnCar(Car& car) const
  {
    static constexpr int ATTEMPT_COUNT = 10;

    for (int i = 0; i < ATTEMPT_COUNT; i++)
    {
      static const Sphere carBoundingSphere = ([&]() {
        Sphere boundingSphere {};
        vec3 min = vec3::zero;
        vec3 max = vec3::zero;

        for (Sphere s : config.collisionGeometries.carSpheres)
        {
          min.x = std::min(min.x, s.center.x - s.radius);
          min.y = std::min(min.y, s.center.y - s.radius);
          min.z = std::min(min.z, s.center.z - s.radius);

          max.x = std::max(max.x, s.center.x + s.radius);
          max.y = std::max(max.y, s.center.y + s.radius);
          max.z = std::max(max.z, s.center.z + s.radius);
        }

        return Sphere {
          .center = 0.5f * (min + max),
          .radius = std::max({max.x - min.x, max.y - min.y, max.z - min.z}),
        };
      })();

      vec2 pos2d = vec2::randomInSquare(Terrain::TERRAIN_SIZE_2 - 2 * carBoundingSphere.radius);
      float bottomOffsetY = -config.physics.car.connectionPoints.wheels.frontLeft.y + config.physics.frontWheels.radius;
      vec3 normal;
      car.position = pos2d.intoXZWithY(terrain.getHeight(pos2d, &normal) + bottomOffsetY);
      car.rotation = quat::fromAxisAngle(normal, randf(2.0f * PI));
      Sphere carBoundingSphereWorld = carBoundingSphere;
      carBoundingSphereWorld.center = carBoundingSphere.center.rotatedBy(car.rotation) + car.position;

      if (!terrain.collideSphereWithObjects(carBoundingSphereWorld, nullptr, nullptr, nullptr))
        break;
    }

    car.resetToPosition(car.position, car.rotation);
  }

  void Scene::regenerateTerrain(Terrain::Mode mode)
  {
    terrain.generate(mode);
    Car& player = cars[localPlayerIndex];
    reset(player.position, player.rotation);
  }

  void Scene::reset(vec3 playerPosition, quat playerRotation)
  {
    float terrainY = terrain.getHeight(playerPosition.x, playerPosition.z);
    playerPosition.y = terrainY + 2;
    Car& player = cars[localPlayerIndex];
    player.resetToPosition(playerPosition, playerRotation);
  }

  const Car* Scene::tryGetPlayer(uint64_t guid) const
  {
    for (int i = 0; i < MAX_CARS; i++)
      if (cars.isAlive(i) && cars[i].guid == guid)
        return &cars[i];

    return nullptr;
  }

  Car* Scene::tryGetPlayer(uint64_t guid)
  {
    for (int i = 0; i < MAX_CARS; i++)
      if (cars.isAlive(i) && cars[i].guid == guid)
        return &cars[i];

    return nullptr;
  }

  void Scene::updatePlayerControl(const PlayerControl& playerControl)
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.isAlive(i) && cars[i].guid == playerControl.guid)
      {
        cars[i].updateControl(playerControl);
        return;
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
  }

  PlayerState Scene::getPlayerState(int index) const
  {
    const Car& player = cars[index];

    PlayerState state = player.getState();
    state.physicalFrame = localPhysicalFrame;
    state.guid = player.guid;

    return state;
  }

  std::vector<PlayerHit> Scene::getPlayerHits(int index) const
  {
    std::vector<PlayerHit> result;
    const Car& player = cars[index];
    const SemiVector<Hit, MAX_CARS>& playerHits = hits[index];

    for (int i = 0; i < playerHits.size(); i++)
    {
      const Hit& hit = playerHits[i];

      // TODO: Consider replacing with guid to avoid issues when index is reused by new connected player
      const Car& attacker = cars[hit.attackerIndex];

      bool isNewHit = true;

      for (int j = 0; j < result.size(); j++)
        if (attacker.guid == result[j].attakerGuid)
        {
          result[j].damage += hit.damage;
          isNewHit = false;

          break;
        }

      if (isNewHit)
        result.push_back(PlayerHit {
          .physicalFrame = localPhysicalFrame,
          .guid = player.guid,
          .attakerGuid = attacker.guid,
          .damage = hit.damage
        });
    }

    return result;
  }

  std::vector<PlayerKill> Scene::getPlayerKills() const
  {
    std::vector<PlayerKill> result;

    for (int i = 0; i < kills.size(); i++)
    {
      const Scene::Kill& kill = kills[i];

      result.push_back(PlayerKill {
        .physicalFrame = localPhysicalFrame,
        .guid = kill.playerGuid,
        .killerGuid = kill.killerGuid
      });
    }

    return result;
  }

}