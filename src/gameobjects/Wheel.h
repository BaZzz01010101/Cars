#pragma once
#include "Terrain.h"
#include "Config.h"
#include "DynamicObject.h"
#include "WheelState.h"

namespace game
{
  using namespace dto;

  struct Wheel : public DynamicObject
  {
    friend struct Hud;

    const Config& config {};
    const Config::Physics::Wheels& wheelConfig {};
    const Terrain& terrain;

    vec3 connectionPoint = vec3::zero;
    vec3 suspecsionForce = vec3::zero;
    vec3 nForce = vec3::zero;
    vec3 frictionForce = vec3::zero;
    vec3 frictionVelocity = vec3::zero;
    quat wheelRotation = quat::identity;
    float momentOfInertia = 0;
    float suspensionOffset = 0;
    float suspensionSpeed = 0;
    float wheelRotationSpeed = 0;
    bool isGrounded = false;

    const char* debugName = "UNKNOWN";

    Wheel(const Config& config, bool isFrontWheel, const Terrain& terrain, vec3 connectionPoint, const char* debugName);
    Wheel(Wheel&) = delete;
    Wheel(Wheel&&) = delete;
    Wheel& operator=(Wheel&) = delete;
    Wheel& operator=(Wheel&&) = delete;

    void update(float dt, const DynamicObject& parent, float steeringAngle, float sharedMass, float enginePower, bool handBreaked);
    void reset();
    WheelState getState() const;
    void syncState(const WheelState& wheelState, float syncFactor, float steeringAngle, const DynamicObject& parent);
  };

}
