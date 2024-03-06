#pragma once
#include "RigidBody.h"
#include "Terrain.h"
#include "Turret.h"
#include "Wheel.h"
#include "Config.h"
#include "Renderable.h"
#include "PhysicalObject.h"
#include "CustomCamera.h"

namespace game
{
  class Car : public Renderable, public PhysicalObject
  {
  public:
    Turret gun{};
    Turret cannon{};

    Wheel frontLeftWheel{};
    Wheel frontRightWheel{};
    Wheel rearLeftWheel{};
    Wheel rearRightWheel{};
    float enginePower{};

    Car();
    void init(Config config, Model carModel, Model wheelModel, Model gunModel, Model cannonModel, const Terrain& terrain, const CustomCamera& camera);
    void update(float dt);
    void draw(bool drawWires);

    void resetToPosition(vec3 position, quat rotation);
    bool traceRay(vec3 origin, vec3 direction, float distance, vec3* collision, vec3* normal) const;

  private:
    const Terrain* terrain{};
    const CustomCamera* camera{};

    vec3 gunConnectionPoint{};
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

    void updateControl(float dt);
    void updateWheels(float dt);
    void updateTurrets(float dt);
    void updateCollisions(float dt);
    vec3 getAutoAlignmentMoment(float dt);
    void drawDebug();
  };

}

