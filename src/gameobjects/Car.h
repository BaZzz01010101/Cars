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
    float enginePower {};
    bool handBreaked {};
    bool gunFiring = false;
    bool cannonFiring = false;
    float timeToNextGunFire = 0;
    float timeToNextCannonFire = 0;
    vec3 gunRecoilForce = vec3::zero;
    vec3 cannonRecoilForce = vec3::zero;
    float steeringDirection {};
    float enginePowerDirection {};
    float verticalTrust {};
    float steeringMaxCorrectionAngle = 0;
    float steeringAngle = 0;
    int health = 0;

    Car(uint64_t guid, const Config& config, const Scene& scene);
    Car(Car&) = delete;
    Car(Car&&) = delete;
    Car& operator=(Car&) = delete;
    Car& operator=(Car&&) = delete;

    void update(float dt);
    void updateControl(const PlayerControl& playerControl);
    PlayerState getState() const;
    void syncState(const PlayerState& playerState, float syncFactor);
    void resetToPosition(vec3 position, quat rotation);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    void updateSteering(float dt);
    void updateWheels(float dt);
    void updateCollisions(float dt);
    vec3 getAutoAlignmentMoment(float dt);
    void updateEngine(float dt);
  };

}

