#pragma once
#include "Terrain.h"
#include "Config.h"
#include "Renderable.h"
#include "PhysicalObject.h"

namespace game
{
  class Wheel : public Renderable, public Object
  {
    friend class Hud;

  public:
    bool isGrounded{};
    vec3 force{};
    vec3 connectionPoint{};

    Wheel(const Config& config, bool isFrontWheel, const Model& model, const Terrain& terrain, vec3 connectionPoint, const char* debugName);
    Wheel(Wheel&) = delete;
    Wheel(Wheel&&) = delete;
    Wheel& operator=(Wheel&) = delete;
    Wheel& operator=(Wheel&&) = delete;

    void update(float dt, const DynamicObject& parent, float steeringAngle, float sharedMass, float enginePower, float brakePower, bool handBreaked);
    void reset();
    void draw(bool drawWires);

  private:
    const Config& config{};
    const Config::Physics::Wheels& wheelConfig{};
    const Terrain& terrain{};
    float momentOfInertia{};

    vec3 nForce{};
    vec3 velocity{};
    vec3 frictionForce{};
    vec3 frictionVelocity{};
    float suspensionOffset{};
    float suspensionSpeed{};
    float wheelRotationSpeed{};
    quat wheelRotation = quat::identity;
    const char* debugName = "UNKNOWN";

    void drawDebug();
  };

}

