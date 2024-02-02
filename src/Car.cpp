#include "pch.h"
#include "Car.h"
#include "Helpers.h"

namespace game
{

  Car::Car()
  {
    frontLeftWheelConnectionPoint = { 0.97f, -0.36f, 1.34f };
    frontRightWheelConnectionPoint = { -0.97f, -0.36f, 1.34f };
    rearLeftWheelConnectionPoint = { 0.97f, -0.536f, -1.20f };
    rearRightWheelConnectionPoint = { -0.97f, -0.536f, -1.20f };

    size = { 2.04f, 2.32f, 4.56f };
    //wheelSize = { 0.327f, 0.925f, 0.925f };
  }

  void Car::init(Config config, Model carModel, Model wheelModel, Model turretModel)
  {
    mass = config.physics.car.mass;
    gravity = config.physics.gravity;
    carConfig = config.physics.car;

    frontLeftWheel.init(config.physics.frontWheels, wheelModel, "FrontLeftWheel", gravity);
    frontRightWheel.init(config.physics.frontWheels, wheelModel, "FrontRightWheel", gravity);
    rearLeftWheel.init(config.physics.rearWheels, wheelModel, "RearLeftWheel", gravity);
    rearRightWheel.init(config.physics.rearWheels, wheelModel, "RearRightWheel", gravity);

    float radius = (size.x + size.y + size.z) / 6;
    momentOfInertia = 0.5f * mass * sqr(radius);

    Renderable::init(carModel);
  }

  void Car::resetToPosition(vec3 position, quat rotation)
  {
    this->position = position;
    this->rotation = rotation;
    velocity = vec3::zero;
    angularVelocity = vec3::zero;
    force = vec3::zero;
    moment = vec3::zero;
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

  void Car::update(float dt, const Terrain& terrain)
  {
    lastForce = force;
    resetForces();
    applyGravity();

    float aerodinamicForce = 5.7f * velocity.sqLength();
    applyGlobalForceAtCenterOfMass(-aerodinamicForce * up());

    vec3 alignmentMoment = getAutoAlignmentMoment(dt);
    applyMoment(alignmentMoment);

    vec3 airFrictionMoment = -angularVelocity / dt * momentOfInertia * 0.001f;
    applyMoment(airFrictionMoment);

    updateControl(dt);

    quat wheelRotation = rotation * quat::fromEuler(steeringAngle, 0, 0);

    frontLeftWheelForce = getWheelForce(dt, frontLeftWheel, terrain, frontLeftWheelConnectionPoint, wheelRotation, 2 * enginePower, brakePower);
    frontRightWheelForce = getWheelForce(dt, frontRightWheel, terrain, frontRightWheelConnectionPoint, wheelRotation, 2 * enginePower, brakePower);
    rearLeftWheelForce = getWheelForce(dt, rearLeftWheel, terrain, rearLeftWheelConnectionPoint, rotation, 1 * enginePower, brakePower);
    rearRightWheelForce = getWheelForce(dt, rearRightWheel, terrain, rearRightWheelConnectionPoint, rotation, 1 * enginePower, brakePower);

    vec3 nForce = frontLeftWheel.nForce + frontRightWheel.nForce + rearLeftWheel.nForce + rearRightWheel.nForce;
    vec3 gravityForce = mass * vec3{ 0, -gravity, 0 };

    applyGlobalForceAtLocalPoint(frontLeftWheelForce, frontLeftWheelConnectionPoint);
    applyGlobalForceAtLocalPoint(frontRightWheelForce, frontRightWheelConnectionPoint);
    applyGlobalForceAtLocalPoint(rearLeftWheelForce, rearLeftWheelConnectionPoint);
    applyGlobalForceAtLocalPoint(rearRightWheelForce, rearRightWheelConnectionPoint);

    updateCollisions(dt, terrain);

    updateBody(dt);
    updateWheels(dt, terrain);
  }

  vec3 Car::getWheelForce(float dt, Wheel& wheel, const Terrain& terrain, vec3 connectionPoint, quat wheelRotation, float enginePower, float brakePower)
  {
    //vec3 positionGlobal = position + connectionPoint.rotatedBy(rotation);
    //vec3 normal = 0.25f * (frontLeftWheel.normal + frontRightWheel.normal + rearLeftWheel.normal + rearRightWheel.normal);
    //vec3 dv = vec3{ 0, -9.8f * dt, 0 }.projectedOnPlane(normal);
    //vec3 velocityGlobal = (velocity + dv) + (angularVelocity % connectionPoint).rotatedBy(rotation);
    wheel.carForward = forward();

    return wheel.getForce(dt, mass / 4, 0, enginePower, brakePower, handBreaked);
  }

  void Car::updateWheels(float dt, const Terrain& terrain)
  {
    float maxCorrectionAngle = (frontLeftWheelConnectionPoint - 0.5f * (rearLeftWheelConnectionPoint + rearRightWheelConnectionPoint)).getYAngle();
    float correctionAngle = fabs(mapRangeClamped(steeringAngle, -PI / 2, PI / 2, -maxCorrectionAngle, maxCorrectionAngle));

    frontLeftWheel.update(dt, terrain, *this, frontLeftWheelConnectionPoint, steeringAngle + correctionAngle);
    frontRightWheel.update(dt, terrain, *this, frontRightWheelConnectionPoint, steeringAngle - correctionAngle);
    rearLeftWheel.update(dt, terrain, *this, rearLeftWheelConnectionPoint, 0);
    rearRightWheel.update(dt, terrain, *this, rearRightWheelConnectionPoint, 0);
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

  void Car::updateCollisions(float dt, const Terrain& terrain)
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
      float terrainY = terrain.getHeight2(ptGlobal.x, ptGlobal.z, &normal);
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
        nForceScalar = std::min(nForceScalar, 100000.0f);
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

  void Car::updateControl(float dt)
  {
    vec3 thrust = {
      0,
      mass * 20 * float(IsKeyDown(KEY_ENTER)),
      0
    };

    handBreaked = IsKeyDown(KEY_SPACE);

    float step = float(IsKeyDown(KEY_HOME) - IsKeyDown(KEY_END)) == sign(enginePower) ? carConfig.enginePower * dt : carConfig.enginePower * 4 * dt;
    float maxEnginePower = mapRangeClamped(velocity.length(), 0, carConfig.maxSpeed, carConfig.enginePower, 0);
    enginePower = moveTo(enginePower, maxEnginePower * float(IsKeyDown(KEY_HOME) - IsKeyDown(KEY_END)), step);
    //brakePower = carConfig.brakePower * float(IsKeyDown(KEY_END));

    applyGlobalForceAtCenterOfMass(thrust);

    float steeringDirection = float(IsKeyDown(KEY_DELETE) - IsKeyDown(KEY_PAGE_DOWN));
    float maxSteeringAngle = mapRangeClamped(velocity * forward(), 0, carConfig.maxSpeed, carConfig.maxSteeringAngle, 0.1f * carConfig.maxSteeringAngle);
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

    drawDebug();
  }

  void Car::drawDebug()
  {
    drawVector(position, 3 * forward(), WHITE);
    drawVector(position, 3 * left(), LIGHTGRAY);
    drawVector(position, 3 * up(), DARKGRAY);

    drawVector(position, 0.5f * force.logarithmic(), RED);
    drawVector(position, 0.5f * moment.logarithmic(), BLUE);

    vec3 frontLeftWheelConnectionPointGlobal = position + frontLeftWheelConnectionPoint.rotatedBy(rotation);
    vec3 frontRightWheelConnectionPointGlobal = position + frontRightWheelConnectionPoint.rotatedBy(rotation);
    vec3 rearLeftWheelConnectionPointGlobal = position + rearLeftWheelConnectionPoint.rotatedBy(rotation);
    vec3 rearRightWheelConnectionPointGlobal = position + rearRightWheelConnectionPoint.rotatedBy(rotation);

    drawVector(frontLeftWheelConnectionPointGlobal, 0.5f * frontLeftWheelForce.logarithmic(), RED);
    drawVector(frontRightWheelConnectionPointGlobal, 0.5f * frontRightWheelForce.logarithmic(), RED);
    drawVector(rearLeftWheelConnectionPointGlobal, 0.5f * rearLeftWheelForce.logarithmic(), RED);
    drawVector(rearRightWheelConnectionPointGlobal, 0.5f * rearRightWheelForce.logarithmic(), RED);
  }
}

