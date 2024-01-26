#pragma once
#include "RigidBody.h"
#include "Terrain.h"
#include "Wheel.h"
#include "Config.h"

namespace game
{

  class Car : public RigidBody, public Renderable
  {
  public:
    Wheel frontLeftWheel{};
    Wheel frontRightWheel{};
    Wheel rearLeftWheel{};
    Wheel rearRightWheel{};

    Car();
    void init(Config config, Model carModel, Model wheelModel, Model turretModel);
    void update(float dt, const Terrain& terrain);
    void draw(bool drawWires);

    void resetToPosition(vec3 position, quat rotation);

  private:
    vec3 frontLeftWheelConnectionPoint{};
    vec3 frontRightWheelConnectionPoint{};
    vec3 rearLeftWheelConnectionPoint{};
    vec3 rearRightWheelConnectionPoint{};

    Config::Physics::Car config{};

    float enginePower{};
    float brakePower{};
    float steeringSpeed{};
    float steeringAngle{};

    vec3 frontLeftWheelForce;
    vec3 frontRightWheelForce;
    vec3 rearLeftWheelForce;
    vec3 rearRightWheelForce;

    void drawDebug();
    void updateControl(float dt);
    vec3 updateWheelForces(float dt, Wheel& wheel, const Terrain& terrain, vec3 connectionPoint, quat wheelRotation, float enginePower, float brakePower);
    void updateCollisions(float dt, const Terrain& terrain);
  };

}

