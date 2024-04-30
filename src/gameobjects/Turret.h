#pragma once
#include "DynamicObject.h"
#include "Config.h"

namespace game
{
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
    vec3 barrelPosition() const;
    vec3 barrelPosition(float lerpFactor) const;
    void update(float dt, const Object& parent);
    void syncState(float yaw, float pitch, float syncFactor, const Object& parent);
  };

}
