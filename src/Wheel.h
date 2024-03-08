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

    void init(const Config::Physics::Wheels& config, const Model& model, const Terrain& terrain, const DynamicObject& parent, vec3 connectionPoint, const char* debugName, float gravity);
    void update(float dt, float steeringAngle, float sharedMass, float enginePower, float brakePower, bool handBreaked);
    void reset();
    void draw(bool drawWires);

  private:
    Config::Physics::Wheels wheelConfig{};
    const Terrain* terrain{};
    const DynamicObject* parent{};
    float gravity{};
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

