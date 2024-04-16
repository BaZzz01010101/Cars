#pragma once
#include "DynamicObject.h"
#include "Config.h"

namespace game
{

  struct PhysicalObject : public DynamicObject
  {
    float mass = 0;
    float momentOfInertia = 0;

    vec3 force = vec3::zero;
    vec3 moment = vec3::zero;

    void update(float dt);
    void resetForces();
    void applyGlobalForceAtLocalPoint(vec3 globalForce, vec3 localPoint);
    void applyGlobalForceAtGlobalPoint(vec3 force, vec3 point);
    void applyGlobalForceAtCenterOfMass(vec3 force);
    void applyMoment(const vec3 moment);
  };

}
