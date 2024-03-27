#pragma once
#include "Terrain.h"
#include "Turret.h"
#include "Wheel.h"
#include "Config.h"
#include "Renderable.h"
#include "PhysicalObject.h"
#include "CustomCamera.h"
#include "PlayerControl.h"
#include "PlayerState.h"

namespace game
{
  class Car : public Renderable, public PhysicalObject
  {
    friend class Hud;

  public:
    float enginePower {};
    bool handBreaked {};
    float steeringDirection {};
    float enginePowerDirection {};
    float verticalTrust {};
    float steeringSpeed = 0;
    float steeringAngle = 0;

    Turret gun;
    Turret cannon;

    Wheel frontLeftWheel;
    Wheel frontRightWheel;
    Wheel rearLeftWheel;
    Wheel rearRightWheel;

    Car(const Config& config, const Model& carModel, const Model& wheelModel, const Model& gunModel, const Model& cannonModel, const Terrain& terrain, const CustomCamera& camera);
    Car(Car&) = delete;
    Car(Car&&) = delete;
    Car& operator=(Car&) = delete;
    Car& operator=(Car&&) = delete;

    void update(float dt);
    void draw(bool drawWires);
    void updateControl(const PlayerControl& playerControl);
    void syncState(const PlayerState& playerState);

    void resetToPosition(vec3 position, quat rotation);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;

  private:
    const Config& config {};
    const Config::Physics::Car& carConfig {};
    const float& gravity {};
    const Terrain& terrain;
    const CustomCamera& camera;

    void updateSteering(float dt);
    void updateWheels(float dt);
    void updateTurrets(float dt);
    void updateCollisions(float dt);
    vec3 getAutoAlignmentMoment(float dt);
    void updateEngine(float dt);
    void drawDebug();
  };

}

