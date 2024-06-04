#pragma once
#include "Turret.h"
#include "Wheel.h"
#include "Config.h"
#include "PhysicalObject.h"
#include "CustomCamera.h"
#include "PlayerControl.h"
#include "PlayerState.h"

namespace game
{
  struct Scene;

  using namespace dto;

  struct Car : public PhysicalObject
  {
    friend struct Hud;

    enum AliveState
    {
      Unknown = 0,
      Countdown,
      Alive,
      Dead,
      Hidden,
    };

    struct SteeringAngles
    {
      float frontLeft;
      float frontRight;
      float rearLeft;
      float rearRight;
    };

    typedef std::pair<AliveState, float> AliveStateRecord;

    static constexpr float DUMMY_HIDE_DURATION = 10.0f;
    static constexpr float PLAYER_DEATH_DURATION = 10.0f;
    static constexpr float DUMMY_ALIVE_DURATION = 10.0f;
    static constexpr float PLAYER_RESPAWN_COUNTDOWN_DURATION = 5.0f;

    static constexpr std::array<AliveStateRecord, 4> aliveStatesOrdered = {
      AliveStateRecord { AliveState::Hidden, DUMMY_HIDE_DURATION },
      AliveStateRecord { AliveState::Dead, PLAYER_DEATH_DURATION },
      AliveStateRecord { AliveState::Alive, DUMMY_ALIVE_DURATION },
      AliveStateRecord { AliveState::Countdown, PLAYER_RESPAWN_COUNTDOWN_DURATION },
    };

    const Config& config {};
    const Config::Physics::Car& carConfig {};
    const float& gravity {};
    const Scene& scene;

    Turret gun;
    Turret cannon;

    Wheel frontLeftWheel;
    Wheel frontRightWheel;
    Wheel rearLeftWheel;
    Wheel rearRightWheel;

    uint64_t guid {};
    PlayerName name = DEFAULT_PLAYER_NAME;
    float enginePower {};
    bool handBreaked {};
    bool gunFiring = false;
    bool cannonFiring = false;
    float timeToNextGunFire = 0;
    float timeToNextCannonFire = 0;
    vec3 gunRecoilForce = vec3::zero;
    vec3 cannonRecoilForce = vec3::zero;
    vec3 hitForce = vec3::zero;
    vec3 hitMoment = vec3::zero;
    float steeringAxis {};
    float accelerationAxis {};
    float thrustAxis {};

    float steeringAngle = 0;
    int health = 0;
    AliveState aliveState {};
    float aliveStateTimeout = 0;

    Car(uint64_t guid, const Config& config, const Scene& scene);
    Car(Car&) = delete;
    Car(Car&&) = delete;
    Car& operator=(Car&) = delete;
    Car& operator=(Car&&) = delete;

    void update(float dt);
    void updateControl(const PlayerControl& playerControl);
    void blockControl();
    PlayerState getState() const;
    void syncState(const PlayerState& playerState, float syncFactor);
    void resetToPosition(vec3 position, quat rotation);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    void updateSteering(float dt);
    void updateWheels(float dt);
    void updateCollisions(float dt);
    vec3 getAutoAlignmentMoment(float dt);
    void updateEngine(float dt);
    SteeringAngles calcSteeringAngles() const;
    void updateAliveStateTimeout(float dt);
    void updateAliveState();
    float getAliveStateTimeout() const;
    float getAliveStateTimeoutProgress() const;
    void switchToAliveState(AliveState newState);
  };

}

