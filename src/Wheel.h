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
    bool isGrounded = false;
    vec3 suspecsionForce = vec3::zero;
    vec3 connectionPoint = vec3::zero;

    Wheel(const Config& config, bool isFrontWheel, const Model& model, const Terrain& terrain, vec3 connectionPoint, const char* debugName);
    Wheel(Wheel&) = delete;
    Wheel(Wheel&&) = delete;
    Wheel& operator=(Wheel&) = delete;
    Wheel& operator=(Wheel&&) = delete;

    void update(float dt, const DynamicObject& parent, float steeringAngle, float sharedMass, float enginePower, bool handBreaked);
    void reset();
    void draw(bool drawWires);

  private:
    const Config& config{};
    const Config::Physics::Wheels& wheelConfig{};
    const Terrain& terrain;
    float momentOfInertia = 0;

    vec3 nForce = vec3::zero;
    vec3 velocity = vec3::zero;
    vec3 frictionForce = vec3::zero;
    vec3 frictionVelocity = vec3::zero;
    float suspensionOffset = 0;
    float suspensionSpeed = 0;
    float wheelRotationSpeed = 0;
    quat wheelRotation = quat::identity;
    const char* debugName = "UNKNOWN";

    void drawDebug();
  };

}

