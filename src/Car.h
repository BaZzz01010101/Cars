#pragma once
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
    friend class Hud;

  public:
    Turret gun{};
    Turret cannon{};

    Wheel frontLeftWheel{};
    Wheel frontRightWheel{};
    Wheel rearLeftWheel{};
    Wheel rearRightWheel{};

    Car();
    void init(const Config& config, Model carModel, Model wheelModel, Model gunModel, Model cannonModel, const Terrain& terrain, const CustomCamera& camera);
    void update(float dt);
    void draw(bool drawWires);

    void resetToPosition(vec3 position, quat rotation);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* normal) const;

  private:
    Config::Physics::Car carConfig{};
    const Terrain* terrain{};
    const CustomCamera* camera{};

    float enginePower{};
    float brakePower{};
    bool handBreaked{};
    float steeringSpeed{};
    float steeringAngle{};

    void updateControl(float dt);
    void updateWheels(float dt);
    void updateTurrets(float dt);
    void updateCollisions(float dt);
    vec3 getAutoAlignmentMoment(float dt);
    void drawDebug();
  };

}

