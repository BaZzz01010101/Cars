#pragma once

namespace game
{

  class RigidBody
  {
  public:
    static constexpr float GRAVITY = -9.81f;
    const char* debugName = "None";

    vec3 position{};
    quat rotation = quat::identity;

    vec3 velocity{};
    vec3 angularVelocity{};

    vec3 force{};
    vec3 moment{};

    float mass{};
    float momentOfInertia{};

    vec3 size{};

    void init();
    void updateBody(float dt);
    void resetForces();
    void applyForceLocal(vec3 force, vec3 point);
    void applyForceLocal(vec3 force);
    void applyForceGlobal(vec3 force, vec3 point);
    void applyGlobalForceAtLocalPoint(vec3 globalForce, vec3 localPoint);
    void applyForceGlobal(vec3 force);
    void applyMoment(const vec3 moment);
    void applyImpulseLocal(vec3 impulse, vec3 point);
    void applyImpulseGlobal(vec3 impulse, vec3 point);
    void applyGravity();
    vec3 forward() const;
    vec3 left() const;
    vec3 up() const;
  };

}