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

    if (isServer)
    {
      clearHits();
      clearKills();
    }

    if (matchState != Scoreboard)
    {
      //// Kills 1st player for debug purposes
      // if (isServer && IsKeyDown(KEY_K))
      //   hits->add({ .tick = 0, .attackerIndex = 0, .damage = 100 });

      updateProjectiles(dt);
      updateCars(dt);

      if (!isServer)
        updateExplosionParticles(dt);
    }

    if (isServer && matchState == Running)
    {
      applyHits();
      applyKills();
      updateRespawn();
    }

    updateMatchTimeout(dt);
  }

  void Scene::updateProjectiles(float dt)
  {
    for (int i = 0; i < projectiles.capacity(); i++)
      if (projectiles.exists(i))
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
        else if (traceRay(projectile.lastPosition, direction, distance, projectile.ownerGuid, &hitPosition, &normal, &hitDistance, &hitCarIndex))
        {
          projectiles.remove(i);

          if (!isServer)
          {
            Config::Graphics::ExplosionParticles explosionConfig = projectile.type == Projectile::Bullet ?
              config.graphics.bulletExplosionParticles :
              config.graphics.shellExplosionParticles;

            createExplosion(explosionConfig, hitPosition);
          }

          if (isServer && hitCarIndex >= 0 && matchState == Running)
          {
            Car& hitCar = cars[hitCarIndex];
            hitCar.hitForce += direction * hitCar.mass * float(projectile.baseDamage) * 0.1f;
            hitCar.hitMoment += ((hitPosition - hitCar.position) % hitCar.hitForce).rotatedBy(hitCar.rotation.inverted());
            uint64_t hitTick = localPhysicalFrame * 100 + int(100 * hitDistance / distance);
            addHit(hitCarIndex, { hitTick, projectile.ownerGuid, projectile.baseDamage });
          }
        }
      }
  }

  void Scene::updateCars(float dt)
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.exists(i))
      {
        Car& car = cars[i];
        car.update(dt);
        updateFiring(i, dt);
      }
  }

  void Scene::updateExplosionParticles(float dt)
  {
    for (int i = 0; i < explosionParticles.capacity(); i++)
      if (explosionParticles.exists(i))
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
          .ownerGuid = car.guid,
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
          .ownerGuid = car.guid,
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
          .ownerGuid = car.guid,
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
      if (cars.exists(i))
      {
        Car& car = cars[i];

        if (car.aliveState == Car::Hidden)
          respawnPlayer(car, true);
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

  bool Scene::traceRay(vec3 origin, vec3 directionNormalized, float distance, uint64_t excludePlayerGuid, vec3* hitPosition, vec3* hitNormal, float* hitDistance, int* hitCarIndex) const
  {
    vec3 closestsHitPosition = vec3::zero;
    vec3 closestsHitNormal = vec3::zero;
    float closestsHitDistance = FLT_MAX;

    vec3 currentHitPosition = vec3::zero;
    vec3 currentHitNormal = vec3::zero;
    float currentHitDistance = FLT_MAX;

    if (terrain.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentHitNormal, &currentHitDistance) &&
      currentHitDistance < closestsHitDistance)
    {
      closestsHitPosition = currentHitPosition;
      closestsHitNormal = currentHitNormal;
      closestsHitDistance = currentHitDistance;
    }

    for (int i = 0; i < cars.capacity(); i++)
      if (cars.exists(i))
      {
        const Car& car = cars[i];

        if (car.guid != excludePlayerGuid &&
          car.aliveState != Car::Countdown &&
          car.aliveState != Car::Hidden &&
          car.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentHitNormal, &currentHitDistance) &&
          currentHitDistance < closestsHitDistance)
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
    HybridVector<Hit, MAX_CARS>& carHits = hits[carIndex];

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
      if (cars.exists(i))
      {
        Car& player = cars[i];

        if (player.health <= 0)
          continue;

        HybridVector<Hit, MAX_CARS>& carHits = hits[i];

        for (int j = 0; j < carHits.size(); j++)
        {
          Hit& hit = carHits[j];
          player.health -= hit.damage;

          if (player.health <= 0)
          {
            kills.add({ player.guid, hit.attackerGuid });

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
        player->switchToAliveState(Car::Dead);
        player->velocity = vec2::randomInRing(5, 5).intoXZWithY(randf(5, 10));
        player->angularVelocity = vec3::randomInCube(5.0f);
      }
    }
  }

  void Scene::respawnPlayer(Car& car, bool withCountdown) const
  {
    static constexpr int ATTEMPT_COUNT = 100;
    vec3 position;
    quat rotation;

    for (int i = 0; i < ATTEMPT_COUNT; i++)
    {
      static const Sphere carBoundingSphere = calcBoundingSphere(config.collisionGeometries.carSpheres);
      vec2 pos2d = vec2::randomInSquare(Terrain::TERRAIN_SIZE_2 - carBoundingSphere.radius);
      static constexpr float DROP_HEIGHT = 0.25f;
      float bottomOffsetY = -config.physics.car.connectionPoints.wheels.frontLeft.y + config.physics.frontWheels.radius + DROP_HEIGHT;
      position = pos2d.intoXZWithY(terrain.getHeight(pos2d) + bottomOffsetY);
      rotation = quat::fromYAngle(randf(2.0f * PI));
      Sphere boundingSphere = carBoundingSphere.transformedBy(position, rotation);

      if (terrain.collideSphereWithObjects(boundingSphere, nullptr, nullptr, nullptr))
        continue;

      if (isCollidingWithPlayer(boundingSphere, car.guid))
        continue;

      break;
    }

    car.health = config.physics.car.maxHealth;
    car.switchToAliveState(withCountdown ? Car::Countdown : Car::Alive);
    car.resetToPosition(position, rotation);
  }

  void Scene::regenerateTerrain(Terrain::Mode mode)
  {
    terrain.generate(mode);

    if (Car* player = tryGetLocalPlayer())
      resetPlayer(player->position, player->rotation);
  }

  void Scene::clear()
  {
    cars.clear();
    projectiles.clear();
    explosionParticles.clear();

    clearHits();
    clearKills();
  }

  void Scene::reset()
  {
    for (int i = 0; i < MAX_CARS; i++)
      if (cars.exists(i))
      {
        Car& car = cars[i];
        car.health = config.physics.car.maxHealth;
        car.switchToAliveState(Car::Alive);
      }


    projectiles.clear();
    explosionParticles.clear();

    clearHits();
    clearKills();
  }

  void Scene::resetPlayer(vec3 playerPosition, quat playerRotation)
  {
    if (Car* player = tryGetLocalPlayer())
    {
      float terrainY = terrain.getHeight(playerPosition.x, playerPosition.z);
      playerPosition.y = terrainY + 2;
      player->resetToPosition(playerPosition, playerRotation);
    }
  }

  const Car* Scene::tryGetLocalPlayer() const
  {
    return tryGetPlayer(localPlayerGuid);
  }

  Car* Scene::tryGetLocalPlayer()
  {
    return localPlayerIndex >= 0 ? &cars[localPlayerIndex] : nullptr;
  }

  const Car* Scene::tryGetPlayer(uint64_t guid) const
  {
    for (int i = 0; i < MAX_CARS; i++)
      if (cars.exists(i) && cars[i].guid == guid)
        return &cars[i];

    return nullptr;
  }

  Car* Scene::tryGetPlayer(uint64_t guid)
  {
    for (int i = 0; i < MAX_CARS; i++)
      if (cars.exists(i) && cars[i].guid == guid)
        return &cars[i];

    return nullptr;
  }

  void Scene::updatePlayerControl(const PlayerControl& playerControl)
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.exists(i) && cars[i].guid == playerControl.guid)
      {
        Car& car = cars[i];

        if (matchState == Scene::Running && car.aliveState == Car::Alive)
          car.updateControl(playerControl);
        else
          car.blockControl();

        return;
      }
  }

  void Scene::syncPlayerState(const PlayerState& playerState, float syncFactor)
  {
    if (Car* player = tryGetPlayer(playerState.guid))
      player->syncState(playerState, syncFactor);
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

    if (!cars.exists(index))
      return result;

    const Car& player = cars[index];
    const HybridVector<Hit, MAX_CARS>& playerHits = hits[index];

    for (int i = 0; i < playerHits.size(); i++)
    {
      const Hit& hit = playerHits[i];

      if (const Car* attacker = tryGetPlayer(hit.attackerGuid))
      {
        bool isNewHit = true;

        for (int j = 0; j < result.size(); j++)
          if (attacker->guid == result[j].attakerGuid)
          {
            result[j].damage += hit.damage;
            isNewHit = false;

            break;
          }

        if (isNewHit)
          result.push_back(PlayerHit {
            .physicalFrame = localPhysicalFrame,
            .guid = player.guid,
            .attakerGuid = attacker->guid,
            .damage = hit.damage
          });
      }
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

  void Scene::updateMatchTimeout(float dt)
  {
    if (isWaitingForPlayers() && matchState == Scene::Running)
      return;

    matchTimeout = std::max(0.0f, matchTimeout - dt);
    updateMatchState();
  }

  void Scene::updateMatchState()
  {
    float threshold = 0;

    for (auto [state, duration] : matchStagesOrdered)
    {
      if (matchTimeout <= threshold + duration)
      {
        matchState = state;
        return;
      }

      threshold += duration;
    }

    matchState = MatchState::Unknown;
  }

  void Scene::respawnAllPlayers()
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.exists(i))
      {
        Car& car = cars[i];
        respawnPlayer(car, false);
      }
  }

  void Scene::switchToMatchState(MatchState newState)
  {
    float timeout = 0;

    for (auto [state, duration] : matchStagesOrdered)
    {
      timeout += duration;

      if (state == newState)
      {
        matchTimeout = timeout;
        matchState = state;

        return;
      }
    }
  }

  float Scene::getMatchStateTimeout() const
  {
    float threshold = 0;

    for (auto [state, duration] : matchStagesOrdered)
    {
      if (matchTimeout <= threshold + duration)
        return matchTimeout - threshold;

      threshold += duration;
    }

    return 0.0f;
  }

  bool Scene::isWaitingForPlayers() const
  {
    return cars.count() <= 1;
  }

  bool Scene::isCollidingWithPlayer(Sphere sphere, uint64_t excludePlayerGuid) const
  {
    for (int i = 0; i < cars.capacity(); i++)
      if (cars.exists(i))
      {
        const Car& car = cars[i];

        if (car.guid != excludePlayerGuid && sphere.center.distanceTo(car.position) < sphere.radius)
          return true;
      }

    return false;
  }

}