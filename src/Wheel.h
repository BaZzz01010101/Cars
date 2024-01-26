#pragma once
#include "RigidBody.h"
#include "Terrain.h"
#include "Config.h"
#include "Renderable.h"

namespace game
{
  class Wheel : public Renderable
  {
  public:
    vec3 nForce{};

    void init(const Config::Physics::Wheels& config, const Model& model, const char* debugName);
    vec3 update(float dt, const Terrain& terrain, vec3 parentPosition, quat parentRotation, vec3 parentVelocity, float enginePower, float brakePower);
    void draw(bool drawWires);
    void reset();

  private:
    Config::Physics::Wheels config{};

    float suspensionOffset{};
    float suspensionSpeed{};
    vec3 position{};
    quat rotation{};
    float wheelAngularVelocity{};
    quat wheelRotation = quat::identity;
    vec3 frictionForce{};
    bool isGrounded{};
    vec3 frictionVelocity{};
    const char* debugName = "UNKNOWN";
  };

}

