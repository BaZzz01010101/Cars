#include "pch.h"
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

    playerIndex = cars.tryAdd(config, terrain);
    Car& player = cars[playerIndex];
    float h = terrain.getHeight(0, 0);
    player.position = { 0, h + 2, 0 };
  }

  void Scene::update(float dt)
  {
    terrain.traceCount = 0;
    updateFiring(dt);

    thread_local int slowMoCounter = 0;
    const int slowMoCounterMax = 40;
    slowMoCounter = (slowMoCounter + 1) % slowMoCounterMax;

    if (!paused && (!slowMotion || slowMoCounter == 0))
      updateGameObjects(dt);
  }

  void Scene::updateGameObjects(float dt)
  {
    for (int i = 0; i < cars.capacity(); i++)
    {
      Car& car = cars[i];
      car.update(dt);
    }

    for (int i = 0; i < projectiles.capacity(); i++)
      if (projectiles.isAlive(i))
      {
        Projectile& projectile = projectiles[i];
        vec3 begin = projectile.position;
        projectile.update(dt);
        vec3 end = projectile.position;
        vec3 hitPosition, normal;

        if (projectile.lifeTime < 0)
          projectiles.remove(i);
        else if (terrain.traceRay(begin, (end - begin).normalized(), (end - begin).length(), &hitPosition, &normal, nullptr))
        {
          projectiles.remove(i);

          Config::Graphics::ExplosionParticles explosionConfig = projectile.type == Projectile::Bullet ?
            config.graphics.bulletExplosionParticles :
            config.graphics.shellExplosionParticles;

          createExplosion(explosionConfig, hitPosition);
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

  void Scene::updateFiring(float dt)
  {
    if (gunFiring)
    {
      if (timeToNextGunFire <= 0)
      {
        const Config::Physics::Turret& gunConfig = config.physics.gun;
        const Car& player = cars[playerIndex];
        const Turret& gun = player.gun;

        float bulletOffsetfix = gunConfig.projectileSpeed * -timeToNextGunFire;
        vec3 bulletPosition = gun.barrelPosition() + gun.forward() * bulletOffsetfix;
        vec3 barrelOffset = 0.2f * gun.left();

        projectiles.tryAdd(Projectile {
          .position = bulletPosition + barrelOffset,
          .velocity = player.velocity + gun.forward() * gunConfig.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = gunConfig.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = playerIndex,
          .damage = gunConfig.baseDamage,
          .type = Projectile::Type::Bullet,
          });

        projectiles.tryAdd(Projectile {
          .position = bulletPosition - barrelOffset,
          .velocity = player.velocity + gun.forward() * gunConfig.projectileSpeed,
          .gravity = config.physics.gravity,
          .lifeTime = gunConfig.projectileLifeTime,
          .size = 0.05f,
          .ownerIndex = playerIndex,
          .damage = gunConfig.baseDamage,
          .type = Projectile::Type::Bullet,
          });

        timeToNextGunFire += gunConfig.fireInterval;
      }
    }
    else
      timeToNextGunFire = 0;

    if (cannonFiring)
    {
      if (timeToNextCannonFire <= 0)
      {
        const Config::Physics::Turret& cannonConfig = config.physics.cannon;
        const Car& player = cars[playerIndex];
        const Turret& cannon = player.cannon;

        projectiles.tryAdd(Projectile {
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

  void Scene::updateLocalPlayerControl(const PlayerControl& playerControl)
  {
    gunFiring = playerControl.primaryFire;
    cannonFiring = playerControl.secondaryFire;

    Car& player = cars[playerIndex];
    player.updateControl(playerControl);
  }

  void Scene::updateRemotePlayerControl(int index, const PlayerControl& playerControl)
  {
    Car& player = cars[playerIndex];
    player.updateControl(playerControl);
  }

  void Scene::syncRemotePlayerState(int index, const PlayerState& playerState)
  {
    Car& player = cars[index];
    player.syncState(playerState);
  }

}