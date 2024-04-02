#pragma once
#include "Terrain.h"
#include "Car.h"
#include "Config.h"
#include "CustomCamera.h"
#include "Pool.hpp"
#include "Projectile.h"
#include "ExplosionParticle.h"
#include "PlayerControl.h"
#include "PlayerState.h"

namespace game
{
  struct Scene
  {
    friend struct Hud;

    const Config& config {};

    Terrain terrain;
    Pool<Car, 1> cars {};
    Pool<Projectile, 1000> projectiles {};
    Pool<ExplosionParticle, 10000> explosionParticles {};

    bool paused = false;
    bool slowMotion = false;
    vec3 gunRayHit = vec3::zero;
    vec3 cannonRayHit = vec3::zero;
    int playerIndex = 0;

    Scene(const Config& config);
    Scene(Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&) = delete;
    Scene& operator=(Scene&&) = delete;

    void init();
    void update(float dt);
    void regenerateTerrain(Terrain::Mode mode);
    void reset(vec3 playerPosition, quat playerRotation);
    inline const Car& getPlayer() const { return cars[playerIndex]; }
    inline Car& getPlayer() { return cars[playerIndex]; }
    void updatePlayerControl(const PlayerControl& playerControl);
    void syncPlayerState(const PlayerState& playerState);
    void getPlayerState(int index, PlayerState* playerState) const;
    void updateGameObjects(float dt);
    void updateFiring(Car& car, float dt);
    void createExplosion(const Config::Graphics::ExplosionParticles& config, vec3 position);
  };

}