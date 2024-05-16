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
#include "PlayerHit.h"
#include "PlayerKill.h"

namespace game
{
  struct Scene
  {
    friend struct Hud;

    struct Hit
    {
      uint64_t tick {};
      int attackerIndex {};
      int damage {};
    };

    struct Kill
    {
      uint64_t playerGuid {};
      uint64_t killerGuid {};
    };

    const Config& config {};

    static constexpr int MAX_CARS = 4;
    static constexpr int MAX_PROJECTILES = 1000;
    static constexpr int MAX_EXPLOSION_PARTICLES = 10000;

    Terrain terrain;
    Pool<Car, MAX_CARS> cars {};
    Pool<Projectile, MAX_PROJECTILES> projectiles {};
    Pool<ExplosionParticle, MAX_EXPLOSION_PARTICLES> explosionParticles {};
    SemiVector<Hit, MAX_CARS> hits[MAX_CARS] {};
    SemiVector<Kill, MAX_CARS> kills {};

    bool isServer = false;
    bool paused = false;
    bool slowMotion = false;
    vec3 gunRayHit = vec3::zero;
    vec3 cannonRayHit = vec3::zero;
    int localPlayerIndex = -1;
    uint64_t localPlayerGuid = 0;
    int64_t localPhysicalFrame = 0;
    uint64_t serverPhysicalFrame = 0;

    Scene(const Config& config, bool isServer);
    Scene(Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&) = delete;
    Scene& operator=(Scene&&) = delete;

    void init();
    void update(float dt);
    void regenerateTerrain(Terrain::Mode mode);
    void reset(vec3 playerPosition, quat playerRotation);
    
    // TODO: Replace with tryGelLocalPlayer
    inline const Car& getLocalPlayer() const { return cars[localPlayerIndex]; }
    inline Car& getLocalPlayer() { return cars[localPlayerIndex]; }

    const Car* tryGetPlayer(uint64_t guid) const;
    Car* tryGetPlayer(uint64_t guid);
    void updatePlayerControl(const PlayerControl& playerControl);
    void syncPlayerState(const PlayerState& playerState, float syncFactor);
    PlayerState getPlayerState(int index) const;
    std::vector<PlayerHit> getPlayerHits(int index) const;
    std::vector<PlayerKill> getPlayerKills() const;
    void updateGameObjects(float dt);
    void updateFiring(int carIndex, float dt);
    void updateRespawn();
    void createExplosion(const Config::Graphics::ExplosionParticles& config, vec3 position);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, int excludePlayerIndex, vec3* hitPosition, vec3* hitNormal, float* hitDistance, int* hitCarIndex) const;
    void clearKills();
    void clearHits();
    void addHit(int carIndex, Hit hit);
    void applyHits();
    void applyKills();
    void respawnCar(Car& car) const;
  };

}