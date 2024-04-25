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

    static constexpr int MAX_CARS = 4;
    static constexpr int MAX_PROJECTILES = 1000;
    static constexpr int MAX_EXPLOSION_PARTICLES = 10000;

    Terrain terrain;
    Pool<Car, MAX_CARS> cars {};
    Pool<Projectile, MAX_PROJECTILES> projectiles {};
    Pool<ExplosionParticle, MAX_EXPLOSION_PARTICLES> explosionParticles {};

    bool paused = false;
    bool slowMotion = false;
    vec3 gunRayHit = vec3::zero;
    vec3 cannonRayHit = vec3::zero;
    int playerIndex = -1;
    uint64_t playerGuid = 0;
    int64_t physicalFrame = 0;
    uint64_t serverPhysicalFrame = 0;

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
    void syncPlayerState(const PlayerState& playerState, float syncFactor);
    PlayerState getPlayerState(int index) const;
    void updateGameObjects(float dt);
    void updateFiring(int carIndex, float dt);
    void createExplosion(const Config::Graphics::ExplosionParticles& config, vec3 position);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, int excludePlayerIndex, vec3* hitPosition, vec3* hitNormal, float* hitDistance, int* hitCarIndex) const;
  };

}