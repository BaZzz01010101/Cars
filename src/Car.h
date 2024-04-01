#pragma once
#include "Terrain.h"
#include "Turret.h"
#include "Wheel.h"
#include "Config.h"
#include "PhysicalObject.h"
#include "CustomCamera.h"
#include "PlayerControl.h"
#include "PlayerState.h"

namespace game
{
  struct Car : public PhysicalObject
  {
    friend struct Hud;

    const Config& config {};
    const Config::Physics::Car& carConfig {};
    const float& gravity {};
    const Terrain& terrain;

    Turret gun;
    Turret cannon;

    Wheel frontLeftWheel;
    Wheel frontRightWheel;
    Wheel rearLeftWheel;
    Wheel rearRightWheel;

    float enginePower {};
    bool handBreaked {};
    float steeringDirection {};
    float enginePowerDirection {};
    float verticalTrust {};
    float steeringSpeed = 0;
    float steeringAngle = 0;

    Car(const Config& config, const Terrain& terrain);
    Car(Car&) = delete;
    Car(Car&&) = delete;
    Car& operator=(Car&) = delete;
    Car& operator=(Car&&) = delete;

    void update(float dt);
    void updateControl(const PlayerControl& playerControl);
    void syncState(const PlayerState& playerState);
    void resetToPosition(vec3 position, quat rotation);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    void updateSteering(float dt);
    void updateWheels(float dt);
    void updateCollisions(float dt);
    vec3 getAutoAlignmentMoment(float dt);
    void updateEngine(float dt);
  };

}

