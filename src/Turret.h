#pragma once
#include "DynamicObject.h"
#include "Config.h"
#include "Terrain.h"

namespace game
{

  struct Turret : public DynamicObject
  {
    const Config::Physics::Turret& config {};
    const Terrain& terrain;

    vec3 connectionPoint = vec3::zero;
    vec3 expectedTarget = vec3::zero;
    vec3 currentTarget = vec3::zero;
    float scale = 0;
    float yaw = 0;
    float pitch = 0;

    Turret(const Config::Physics::Turret& config, const Terrain& terrain, vec3 parentConnectionPoint, float scale);
    Turret(Turret&) = delete;
    Turret(Turret&&) = delete;
    Turret& operator=(Turret&) = delete;
    Turret& operator=(Turret&&) = delete;

    void reset();
    vec3 barrelPosition() const;
    void update(float dt, const Object& parent);
    void syncState(float yaw, float pitch);
  };

}
