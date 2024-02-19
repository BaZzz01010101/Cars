#pragma once
#include "DynamicObject.h"
#include "Config.h"

namespace game
{

  class PhysicalObject : virtual public DynamicObject
  {
  public:
    float mass{};
    float momentOfInertia{};

    vec3 force{};
    vec3 moment{};

    virtual ~PhysicalObject() = default;

    void init(const Config::Physics& physicsConfig);
    void resetForces();
    void applyGlobalForceAtLocalPoint(vec3 globalForce, vec3 localPoint);
    void applyGlobalForceAtGlobalPoint(vec3 force, vec3 point);
    void applyGlobalForceAtCenterOfMass(vec3 force);
    void applyMoment(const vec3 moment);
    void applyGravity();

    virtual void update(float dt);

  private:
    Config::Physics physicsConfig{};
  };

}
