#pragma once
#include "DynamicObject.h"
#include "Config.h"

namespace game
{

  class PhysicalObject : public DynamicObject
  {
  public:
    float mass{};
    float momentOfInertia{};

    vec3 force{};
    vec3 moment{};

    PhysicalObject(const Config::Physics& physicsConfig);

    void update(float dt);

    void resetForces();

    void applyGlobalForceAtLocalPoint(vec3 globalForce, vec3 localPoint);
    void applyGlobalForceAtGlobalPoint(vec3 force, vec3 point);
    void applyGlobalForceAtCenterOfMass(vec3 force);
    void applyMoment(const vec3 moment);
    void applyGravity();

  private:
    Config::Physics physicsConfig{};
  };

}
