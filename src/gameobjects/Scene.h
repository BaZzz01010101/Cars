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

    enum MatchState
    {
      Unknown = 0,
      Countdown,
      Running,
      Scoreboard,
    };

    struct Hit
    {
      uint64_t tick {};
      uint64_t attackerGuid{};
      int damage {};
    };

    struct Kill
    {
      uint64_t playerGuid {};
      uint64_t killerGuid {};
    };

    typedef std::pair<MatchState, float> MatchStateRecord;

    const Config& config {};

    static constexpr int MAX_CARS = 4;
    static constexpr int MAX_PROJECTILES = 1000;
    static constexpr int MAX_EXPLOSION_PARTICLES = 10000;
    static constexpr float MATCH_COUNTDOWN_DURATION = 9;
    static constexpr float MATCH_RUNNING_DURATION = 5 * 60;
    static constexpr float MATCH_SCOREBOARD_DURATION = 30;

    static constexpr std::array<MatchStateRecord, 3> matchStagesOrdered = {
      MatchStateRecord { MatchState::Scoreboard, MATCH_SCOREBOARD_DURATION },
      MatchStateRecord { MatchState::Running, MATCH_RUNNING_DURATION },
      MatchStateRecord { MatchState::Countdown, MATCH_COUNTDOWN_DURATION },
    };

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
    // TODO: Consider getting rid of local player index in favor of guid
    int localPlayerIndex = -1;
    uint64_t localPlayerGuid = 0;
    int64_t localPhysicalFrame = 0;
    uint64_t serverPhysicalFrame = 0;
    float matchTimeout = 0;
    MatchState matchState {};

    Scene(const Config& config, bool isServer);
    Scene(Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&) = delete;
    Scene& operator=(Scene&&) = delete;

    void init();
    void update(float dt);
    void regenerateTerrain(Terrain::Mode mode);
    void reset();
    void resetPlayer(vec3 playerPosition, quat playerRotation);

    const Car* tryGetLocalPlayer() const;
    Car* tryGetLocalPlayer();
    const Car* tryGetPlayer(uint64_t guid) const;
    Car* tryGetPlayer(uint64_t guid);
    void updatePlayerControl(const PlayerControl& playerControl);
    void syncPlayerState(const PlayerState& playerState, float syncFactor);
    PlayerState getPlayerState(int index) const;
    std::vector<PlayerHit> getPlayerHits(int index) const;
    std::vector<PlayerKill> getPlayerKills() const;
    void updateProjectiles(float dt);
    void updateCars(float dt);
    void updateExplosionParticles(float dt);
    void updateFiring(int carIndex, float dt);
    void updateRespawn();
    void createExplosion(const Config::Graphics::ExplosionParticles& config, vec3 position);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, uint64_t excludePlayerGuid, vec3* hitPosition, vec3* hitNormal, float* hitDistance, int* hitCarIndex) const;
    void clearKills();
    void clearHits();
    void addHit(int carIndex, Hit hit);
    void applyHits();
    void applyKills();
    void respawnPlayer(Car& car, bool withCountdown) const;
    void respawnAllPlayers();
    void updateMatchTimeout(float dt);
    void updateMatchState();
    void switchToMatchState(MatchState newState);
    float getMatchStateTimeout() const;
    bool isWaitingForPlayers() const;
    bool isCollidingWithPlayer(Sphere sphere, uint64_t excludePlayerGuid) const;
  };

}