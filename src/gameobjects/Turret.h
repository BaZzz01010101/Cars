#pragma once
#include "DynamicObject.h"
#include "Config.h"
#include "TurretState.h"

namespace game
{
  using namespace dto;
  struct Scene;

  struct Turret : public DynamicObject
  {
    const Config::Physics::Turret& config {};
    const Scene& scene;

    vec3 connectionPoint = vec3::zero;
    vec3 expectedTarget = vec3::zero;
    vec3 target = vec3::zero;
    float yaw = 0;
    float pitch = 0;

    Turret(const Config::Physics::Turret& config, const Scene& scene, vec3 parentConnectionPoint);
    Turret(Turret&) = delete;
    Turret(Turret&&) = delete;
    Turret& operator=(Turret&) = delete;
    Turret& operator=(Turret&&) = delete;

    void reset();
    vec3 barrelFrontPosition() const;
    vec3 barrelBackPosition() const;
    void update(float dt, const Object& parent);
    TurretState getState() const;
    void syncState(TurretState turretState, float syncFactor, const Object& parent);
  };

}
