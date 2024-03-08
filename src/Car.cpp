#include "pch.h"
#include "Car.h"
#include "Helpers.h"

namespace game
{
  Car::Car(const Config& config, const Model& carModel, const Model& wheelModel, const Model& gunModel, const Model& cannonModel, const Terrain& terrain, const CustomCamera& camera) :
    carConfig(config.physics.car),
    Renderable(carModel),
    PhysicalObject(config.physics),
    gun(config.physics.gun, gunModel, terrain, config.physics.car.connectionPoints.weapon.gun, 1),
    cannon(config.physics.cannon, gunModel, terrain, config.physics.car.connectionPoints.weapon.cannon, 2),
    frontLeftWheel(config, true, wheelModel, terrain, config.physics.car.connectionPoints.wheels.frontLeft, "FrontLeftWheel"),
    frontRightWheel(config, true, wheelModel, terrain, config.physics.car.connectionPoints.wheels.frontRight, "FrontRightWheel"),
    rearLeftWheel(config, false, wheelModel, terrain, config.physics.car.connectionPoints.wheels.rearLeft, "RearLeftWheel"),
    rearRightWheel(config, false, wheelModel, terrain, config.physics.car.connectionPoints.wheels.rearRight, "RearRightWheel"),
    terrain(terrain),
    camera(camera)
  {
    mass = config.physics.car.mass;
    vec3 size = { 2.04f, 2.32f, 4.56f };
    float radius = (size.x + size.y + size.z) / 6;
    momentOfInertia = 0.5f * mass * sqr(radius);
  }

  void Car::resetToPosition(vec3 position, quat rotation)
  {
    this->position = position;
    this->rotation = rotation;
    velocity = vec3::zero;
    angularVelocity = vec3::zero;
    force = vec3::zero;
    moment = vec3::zero;
    enginePower = 0;
    brakePower = 0;
    handBreaked = false;
    steeringAngle = 0.0f;
    frontLeftWheel.reset();
    frontRightWheel.reset();
    rearLeftWheel.reset();
    rearRightWheel.reset();
  }

  bool Car::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* normal) const
  {
    return false;
  }

  void Car::update(float dt)
  {
    resetForces();
    applyGravity();

    float aerodinamicForce = carConfig.aerodynamicKoef * velocity.sqLength();
    applyGlobalForceAtCenterOfMass(-aerodinamicForce * up());

    vec3 alignmentMoment = getAutoAlignmentMoment(dt);
    applyMoment(alignmentMoment);

    vec3 airFrictionMoment = -angularVelocity / dt * momentOfInertia * 0.001f;
    applyMoment(airFrictionMoment);

    updateControl(dt);

    quat wheelRotation = rotation * quat::fromEuler(steeringAngle, 0, 0);

    applyGlobalForceAtLocalPoint(frontLeftWheel.force, frontLeftWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(frontRightWheel.force, frontRightWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(rearLeftWheel.force, rearLeftWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(rearRightWheel.force, rearRightWheel.connectionPoint);

    updateCollisions(dt);

    PhysicalObject::update(dt);
    updateWheels(dt);
    updateTurrets(dt);
  }

  void Car::updateWheels(float dt)
  {
    float maxCorrectionAngle = (frontLeftWheel.connectionPoint - 0.5f * (rearLeftWheel.connectionPoint + rearRightWheel.connectionPoint)).getYAngle();
    float correctionAngle = fabs(mapRangeClamped(steeringAngle, -PI / 2, PI / 2, -maxCorrectionAngle, maxCorrectionAngle));

    float frontPower = 1.0;
    float rearPower = 1.0;
    float contactsCount = std::max(1.0f, float(frontLeftWheel.isGrounded + frontRightWheel.isGrounded + rearLeftWheel.isGrounded + rearRightWheel.isGrounded));
    float sharedMass = mass / contactsCount;

    float frontLeftSteeringAngle = steeringAngle + correctionAngle;
    float frontRightSteeringAngle = steeringAngle - correctionAngle;
    frontLeftWheel.update(dt, *this, frontLeftSteeringAngle, sharedMass, rearPower * enginePower, brakePower, handBreaked);
    frontRightWheel.update(dt, *this, frontLeftSteeringAngle, sharedMass, rearPower * enginePower, brakePower, handBreaked);
    rearLeftWheel.update(dt, *this, 0, sharedMass, rearPower * enginePower, brakePower, handBreaked);
    rearRightWheel.update(dt, *this, 0, sharedMass, rearPower * enginePower, brakePower, handBreaked);
  }

  void Car::updateTurrets(float dt)
  {
    vec3 targetCollisionPosition;
    bool isHit = terrain.traceRay(camera.position, camera.direction, -1, &targetCollisionPosition, nullptr);
    vec3 gunPosition = gun.position + 0.35f * up();
    vec3 cannonPosition = cannon.position + 0.75f * up();

    gun.target = isHit ? (targetCollisionPosition - gunPosition) : camera.direction;
    cannon.target = isHit ? (targetCollisionPosition - cannonPosition) : camera.direction;
    gun.update(dt, *this);
    cannon.update(dt, *this);
  }

  void Car::updateCollisions(float dt)
  {
    const std::vector<vec3> points = {
      {0.81f, 0.11f, 2.34f},
      {0.73f, 1.37f, 0.61f},
      {0.64f, 1.32f, -0.81f},
      {0.68f, 0.43f, -2.19f},
      {0.97f, -0.15f, -1.88f},
      {-0.81f, 0.11f, 2.34f},
      {-0.73f, 1.37f, 0.61f},
      {-0.64f, 1.32f, -0.81f},
      {-0.68f, 0.43f, -2.19f},
      {-0.97f, -0.15f, -1.88f},
    };

    if (IsKeyDown(KEY_F))
    {
      applyMoment({ 0, 0, -10000 });
    }

    typedef std::pair<vec3, vec3> ForceAndPoint;
    std::vector<ForceAndPoint> frictionForces;
    std::vector<ForceAndPoint> nForces;
    float div = 0;

    for (vec3 pt : points)
    {
      vec3 ptRotated = pt.rotatedBy(rotation);
      vec3 ptGlobal = ptRotated + position;
      vec3 normal;
      float terrainY = terrain.getHeight(ptGlobal.x, ptGlobal.z, &normal);
      const float MAX_PENETRATION = 0.1f;
      float penetration = std::max(terrainY - ptGlobal.y, 0.0f) / MAX_PENETRATION;
      vec3 ptRotationfVelocity = (angularVelocity % pt).rotatedBy(rotation);
      vec3 ptVelocity = vec3::zero;
      ptVelocity += velocity;
      ptVelocity += ptRotationfVelocity;

      if (penetration > 0)
      {
        div += 1.0f;
        //position.y += terrainY - ptGlobal.y;
        float nForceScalar = mass / dt * penetration;
        nForceScalar = std::min(nForceScalar, 50000.0f);
        vec3 nForce = normal * nForceScalar;

        vec3 frictionForce = -ptVelocity.projectedOnPlane(normal) / dt / (pt.sqLength() / momentOfInertia + 1 / mass);
        float frictionForceScalar = frictionForce.length();
        float maxFrictionForce = std::min(nForceScalar, 10000.0f) * carConfig.bodyFriction;
        velocity -= velocity.projectedOnVector(normal) * clamp(penetration * dt * 10, 0.0f, 1.0f);

        if (frictionForceScalar > 0 && frictionForceScalar > sqr(maxFrictionForce))
          frictionForce = frictionForce / frictionForceScalar * maxFrictionForce;

        frictionForces.push_back({ frictionForce, ptRotated });
        nForces.push_back({ nForce, ptRotated });
      }
    }

    for (const ForceAndPoint& f : frictionForces)
      applyGlobalForceAtGlobalPoint(f.first / div, f.second);

    for (const ForceAndPoint& f : nForces)
      applyGlobalForceAtGlobalPoint(f.first / div, f.second);
  }

  vec3 Car::getAutoAlignmentMoment(float dt)
  {
    vec3 moment = vec3::zero;

    vec3 axis;
    float angle;
    quat::fromVectorToVector(up(), vec3::up).toAxisAngle(&axis, &angle);

    angle -= clamp(angle, 0.0f, PI / 8);
    float k = mapRangeClamped(angle, 0, PI, 0, 1);
    k = sqr(k) * sign(k);

    vec3 rotationMoment = axis.rotatedBy(rotation.inverted()) * k * momentOfInertia * carConfig.carAligningForce;
    moment += rotationMoment;

    vec3 angularVelocityMoment = -angularVelocity / dt * momentOfInertia * 0.001f * carConfig.carAligningForce;
    angularVelocityMoment.y = 0;

    moment += angularVelocityMoment;

    return moment;
  }

  void Car::updateControl(float dt)
  {
    vec3 thrust = {
      0,
      mass * 20 * float(IsKeyDown(KEY_LEFT_SHIFT)),
      0
    };

    handBreaked = IsKeyDown(KEY_SPACE);

    float step = float(IsKeyDown(KEY_W) - IsKeyDown(KEY_S)) == sign(enginePower) ? carConfig.enginePower * dt : carConfig.enginePower * 4 * dt;
    float maxForwardEnginePower = mapRangeClamped(velocity * forward(), 0, carConfig.maxSpeed, carConfig.enginePower, 0);
    float maxBackwardEnginePower = mapRangeClamped(velocity * forward(), -carConfig.maxSpeed, 0, 0, carConfig.enginePower);
    enginePower = moveTo(enginePower, maxForwardEnginePower * float(IsKeyDown(KEY_W)) - maxBackwardEnginePower * float(IsKeyDown(KEY_S)), step);
    //brakePower = carConfig.brakePower * float(IsKeyDown(KEY_END));

    applyGlobalForceAtCenterOfMass(thrust);

    float steeringDirection = float(IsKeyDown(KEY_A) - IsKeyDown(KEY_D));
    float maxSteeringAngle = mapRangeClamped(velocity * forward(), 0, carConfig.maxSpeed, carConfig.maxSteeringAngle, carConfig.minSteeringAngle);
    float steeringTarget;

    if (steeringDirection == 0.0f)
    {
      steeringDirection = -sign(steeringAngle);
      steeringTarget = 0.0f;
    }
    else
      steeringTarget = maxSteeringAngle * steeringDirection;

    //float maxSteeringSpeed = mapRangeClamped(velocity.length(), 0, carConfig.maxSpeed, carConfig.maxSteeringSpeed, carConfig.maxSteeringSpeed * 0.5f);

    steeringAngle = moveTo(steeringAngle, steeringTarget, carConfig.maxSteeringSpeed * steeringDirection * dt);
  }

  void Car::draw(bool drawWires)
  {
    Matrix transform = MatrixMultiply(QuaternionToMatrix(rotation), MatrixTranslate(position.x, position.y, position.z));
    Renderable::draw(transform, drawWires);

    frontLeftWheel.draw(drawWires);
    frontRightWheel.draw(drawWires);
    rearLeftWheel.draw(drawWires);
    rearRightWheel.draw(drawWires);

    gun.draw(drawWires);
    cannon.draw(drawWires);

    drawDebug();
  }

  void Car::drawDebug()
  {
    //drawVector(position, 3 * forward(), WHITE);
    //drawVector(position, 3 * left(), LIGHTGRAY);
    //drawVector(position, 3 * up(), DARKGRAY);

    //drawVector(position, 0.001f * force, RED);
    //drawVector(position, 0.5f * moment.logarithmic(), BLUE);
    drawVector(position, 5 * vec3::forward, WHITE);
    drawVector(position, 5 * vec3::left, LIGHTGRAY);
  }
}

