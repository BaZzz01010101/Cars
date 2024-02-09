#pragma once
#include "RigidBody.h"
#include "Terrain.h"
#include "Config.h"
#include "Renderable.h"
#include "Physable.h"

namespace game
{
  class Wheel : public Renderable, public Movable
  {
  public:
    bool isGrounded{};
    vec3 normal{};
    vec3 nForce{};
    vec3 carDampingForce{};
    float wheelRotationSpeed{};
    vec3 frictionForce{};
    vec3 force{};

    void init(const Config::Physics::Wheels& config, const Model& model, const char* debugName, float gravity);
    vec3 getForce(float dt, float sharedMass, float enginePower, float brakePower, bool handBreaked);
    void update(float dt, const Terrain& terrain, const Physable& parent, vec3 parentConnectionPoint, float steeringAngle);
    void draw(bool drawWires);
    void drawDebug();
    void reset();

  private:
    float gravity{};
    float momentOfInertia{};
    Config::Physics::Wheels wheelConfig{};

    float suspensionOffset{};
    float suspensionSpeed{};
    quat wheelRotation = quat::identity;
    vec3 frictionVelocity{};
    const char* debugName = "UNKNOWN";
  };

}

