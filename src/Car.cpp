#include "pch.h"
#include "Car.h"
#include "Helpers.h"

namespace game
{
  Car::Car()
  {}

  void Car::init(Config config, Model carModel, Model wheelModel, Model gunModel, Model cannonModel, const Terrain& terrain, const CustomCamera& camera)
  {
    this->terrain = &terrain;
    this->camera = &camera;

    mass = config.physics.car.mass;
    carConfig = config.physics.car;
    vec3 size = { 2.04f, 2.32f, 4.56f };

    gunConnectionPoint = { 0, 0.45f, -0.2f };
    cannonConnectionPoint = { 0, 0.55f, -1.6f };

    frontLeftWheelConnectionPoint = { 0.97f, -0.36f, 1.34f };
    frontRightWheelConnectionPoint = { -0.97f, -0.36f, 1.34f };
    rearLeftWheelConnectionPoint = { 0.97f, -0.536f, -1.20f };
    rearRightWheelConnectionPoint = { -0.97f, -0.536f, -1.20f };

    gun.init(config.physics.gun, gunModel, terrain, *this, gunConnectionPoint, 1);
    cannon.init(config.physics.cannon, cannonModel, terrain, *this, cannonConnectionPoint, 2);

    frontLeftWheel.init(config.physics.frontWheels, wheelModel, terrain, *this, frontLeftWheelConnectionPoint, "FrontLeftWheel", config.physics.gravity);
    frontRightWheel.init(config.physics.frontWheels, wheelModel, terrain, *this, frontRightWheelConnectionPoint, "FrontRightWheel", config.physics.gravity);
    rearLeftWheel.init(config.physics.rearWheels, wheelModel, terrain, *this, rearLeftWheelConnectionPoint, "RearLeftWheel", config.physics.gravity);
    rearRightWheel.init(config.physics.rearWheels, wheelModel, terrain, *this, rearRightWheelConnectionPoint, "RearRightWheel", config.physics.gravity);

    float radius = (size.x + size.y + size.z) / 6;
    momentOfInertia = 0.5f * mass * sqr(radius);

    Renderable::init(carModel);
    PhysicalObject::init(config.physics);
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
    frontLeftWheelForce = vec3::zero;
    frontRightWheelForce = vec3::zero;
    rearLeftWheelForce = vec3::zero;
    rearRightWheelForce = vec3::zero;
  }

  bool Car::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* normal) const
  {
    return false;
  }

  void Car::update(float dt)
  {
    lastForce = force;
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
    float frontPower = 1.5;
    float rearPower = 1.5;
    float contactsCount = std::max(1.0f, float(frontLeftWheel.isGrounded + frontRightWheel.isGrounded + rearLeftWheel.isGrounded + rearRightWheel.isGrounded));
    float sharedMass = mass / contactsCount;
    frontLeftWheelForce = frontLeftWheel.getForce(dt, sharedMass, frontPower * enginePower, brakePower, false);
    frontRightWheelForce = frontRightWheel.getForce(dt, sharedMass, frontPower * enginePower, brakePower, false);
    rearLeftWheelForce = rearLeftWheel.getForce(dt, sharedMass, rearPower * enginePower, brakePower, handBreaked);
    rearRightWheelForce = rearRightWheel.getForce(dt, sharedMass, rearPower * enginePower, brakePower, handBreaked);

    vec3 nForce = frontLeftWheel.nForce + frontRightWheel.nForce + rearLeftWheel.nForce + rearRightWheel.nForce;

    applyGlobalForceAtLocalPoint(frontLeftWheelForce, frontLeftWheelConnectionPoint);
    applyGlobalForceAtLocalPoint(frontRightWheelForce, frontRightWheelConnectionPoint);
    applyGlobalForceAtLocalPoint(rearLeftWheelForce, rearLeftWheelConnectionPoint);
    applyGlobalForceAtLocalPoint(rearRightWheelForce, rearRightWheelConnectionPoint);

    updateCollisions(dt);

    PhysicalObject::update(dt);
    updateWheels(dt);
    updateTurrets(dt);
  }

  void Car::updateWheels(float dt)
  {
    float maxCorrectionAngle = (frontLeftWheelConnectionPoint - 0.5f * (rearLeftWheelConnectionPoint + rearRightWheelConnectionPoint)).getYAngle();
    float correctionAngle = fabs(mapRangeClamped(steeringAngle, -PI / 2, PI / 2, -maxCorrectionAngle, maxCorrectionAngle));

    frontLeftWheel.steeringAngle = steeringAngle + correctionAngle;
    frontRightWheel.steeringAngle = steeringAngle - correctionAngle;
    frontLeftWheel.update(dt);
    frontRightWheel.update(dt);
    rearLeftWheel.update(dt);
    rearRightWheel.update(dt);
  }

  void Car::updateTurrets(float dt)
  {
    vec3 targetCollisionPosition;
    bool isHit = terrain->traceRay(camera->position, camera->direction, -1, &targetCollisionPosition, nullptr);
    vec3 gunPosition = gun.position + 0.35f * up();
    vec3 cannonPosition = cannon.position + 0.75f * up();

    gun.target = isHit ? (targetCollisionPosition - gunPosition) : camera->direction;
    cannon.target = isHit ? (targetCollisionPosition - cannonPosition) : camera->direction;
    gun.update(dt);
    cannon.update(dt);
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
      float terrainY = terrain->getHeight(ptGlobal.x, ptGlobal.z, &normal);
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

