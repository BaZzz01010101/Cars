#pragma once
#include "RigidBody.h"
#include "Terrain.h"
#include "Config.h"
#include "Renderable.h"
#include "PhysicalObject.h"

namespace game
{
  class Wheel : public Renderable, public DynamicObject
  {
  public:
    bool isGrounded{};
    vec3 normal{};
    vec3 nForce{};
    vec3 carDampingForce{};
    float wheelRotationSpeed{};
    vec3 frictionForce{};
    vec3 force{};
    float steeringAngle{};

    void init(const Config::Physics::Wheels& config, const Model& model, const Terrain& terrain, const PhysicalObject& parent, vec3 parentConnectionPoint, const char* debugName, float gravity);
    vec3 getForce(float dt, float sharedMass, float enginePower, float brakePower, bool handBreaked);
    void update(float dt);
    void reset();
    void draw(bool drawWires);

  private:
    const Terrain* terrain{};
    const DynamicObject* parent{};
    vec3 parentConnectionPoint{};
    float gravity{};
    float momentOfInertia{};
    Config::Physics::Wheels wheelConfig{};

    float suspensionOffset{};
    float suspensionSpeed{};
    quat wheelRotation = quat::identity;
    vec3 frictionVelocity{};
    const char* debugName = "UNKNOWN";

    void drawDebug();
  };

}

