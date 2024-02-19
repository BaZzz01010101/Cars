#pragma once
#include "Object.h"
#include "DynamicObject.h"

namespace game
{

  class RigidBody : public DynamicObject
  {
  public:
    const char* debugName = "None";

    float gravity{};

    vec3 force{};
    vec3 moment{};

    float mass{};
    float momentOfInertia{};

    vec3 size{};

    void updateBody(float dt);
    void resetForces();
    void applyLocalForceAtLocalPoint(vec3 force, vec3 point);
    void applyLocalForceAtCenterOfMass(vec3 force);
    void applyGlobalForceAtGlobalPoint(vec3 force, vec3 point);
    void applyGlobalForceAtLocalPoint(vec3 globalForce, vec3 localPoint);
    void applyGlobalForceAtCenterOfMass(vec3 force);
    void applyMoment(const vec3 moment);
    void applyGravity();
  };

}