#pragma once
#include "RigidBody.h"
#include "Terrain.h"
#include "Turret.h"
#include "Wheel.h"
#include "Config.h"
#include "Renderable.h"
#include "Physable.h"

namespace game
{

  class Car : public Renderable, public Physable
  {
  public:
    Turret gun{};
    Turret cannon{};

    Wheel frontLeftWheel{};
    Wheel frontRightWheel{};
    Wheel rearLeftWheel{};
    Wheel rearRightWheel{};
    float enginePower{};

    Car() = default;
    void init(Config config, Model carModel, Model wheelModel, Model turretModel);
    void update(float dt, const Terrain& terrain, vec3 cameraTarget);
    void draw(bool drawWires);

    void resetToPosition(vec3 position, quat rotation);

  private:
    vec3 turretConnectionPoint{};
    vec3 cannonConnectionPoint{};

    vec3 frontLeftWheelConnectionPoint{};
    vec3 frontRightWheelConnectionPoint{};
    vec3 rearLeftWheelConnectionPoint{};
    vec3 rearRightWheelConnectionPoint{};

    Config::Physics::Car carConfig{};

    float brakePower{};
    bool handBreaked{};
    float steeringSpeed{};
    float steeringAngle{};

    vec3 frontLeftWheelForce{};
    vec3 frontRightWheelForce{};
    vec3 rearLeftWheelForce{};
    vec3 rearRightWheelForce{};

    vec3 lastForce{};

    void drawDebug();
    void updateControl(float dt);
    void updateWheels(float dt, const Terrain& terrain);
    vec3 getAutoAlignmentMoment(float dt);
    void updateCollisions(float dt, const Terrain& terrain);
  };

}

