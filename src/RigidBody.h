#pragma once

namespace game
{

  class RigidBody
  {
  public:
    const char* debugName = "None";

    float gravity{};

    vec3 position{};
    quat rotation = quat::identity;
    vec3 eulerRotation{};

    vec3 velocity{};
    vec3 angularVelocity{};

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
    vec3 forward() const;
    vec3 left() const;
    vec3 up() const;
  };

}