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
    this->config = config.physics.car;

    frontLeftWheel.init(config.physics.frontWheels, wheelModel, "FrontLeftWheel");
    frontRightWheel.init(config.physics.frontWheels, wheelModel, "FrontRightWheel");
    rearLeftWheel.init(config.physics.rearWheels, wheelModel, "RearLeftWheel");
    rearRightWheel.init(config.physics.rearWheels, wheelModel, "RearRightWheel");

    float radius = (size.x + size.y + size.z) / 6;
    momentOfInertia = 0.5f * mass * sqr(radius);

    RigidBody::init();
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
    resetForces();
    applyGravity();

    vec3 axis;
    float angle;
    quat::fromVectorToVector(up(), vec3::up).toAxisAngle(&axis, &angle);

    angle -= clamp(angle, 0.0f, PI / 8);
    angle = mapRangeClamped(angle, 0, PI, 0, 1);
    applyMoment(axis.rotatedBy(rotation.inverted()) * sqr(angle) * sign(angle) * momentOfInertia * config.carAligningForce);

    vec3 m = -angularVelocity / dt * momentOfInertia * 0.001f * config.carAligningForce;
    m.y = 0;

    applyMoment(m);

    vec3 airFrictionMoment = -angularVelocity / dt * momentOfInertia * 0.001f;
    applyMoment(airFrictionMoment);

    updateControl(dt);

    if (IsKeyPressed(KEY_R))
    {
      rotation = quat::fromEuler(PI / 2, 0, 0);
      position.y += 0.5;
    }

    applyForceLocal(frontLeftWheelForce, frontLeftWheelConnectionPoint);
    applyForceLocal(frontRightWheelForce, frontRightWheelConnectionPoint);
    applyForceLocal(rearLeftWheelForce, rearLeftWheelConnectionPoint);
    applyForceLocal(rearRightWheelForce, rearRightWheelConnectionPoint);

    updateCollisions(dt, terrain);

    RigidBody::updateBody(dt);

    quat wheelRotation = rotation * quat::fromEuler(steeringAngle, 0, 0);

    frontLeftWheelForce = updateWheelForces(dt, frontLeftWheel, terrain, frontLeftWheelConnectionPoint, wheelRotation, enginePower, brakePower);
    frontRightWheelForce = updateWheelForces(dt, frontRightWheel, terrain, frontRightWheelConnectionPoint, wheelRotation, enginePower, brakePower);
    rearLeftWheelForce = updateWheelForces(dt, rearLeftWheel, terrain, rearLeftWheelConnectionPoint, rotation, enginePower, brakePower);
    rearRightWheelForce = updateWheelForces(dt, rearRightWheel, terrain, rearRightWheelConnectionPoint, rotation, enginePower, brakePower);

    vec3 nForce = frontLeftWheel.nForce + frontRightWheel.nForce + rearLeftWheel.nForce + rearRightWheel.nForce;
    vec3 gravityForce = mass * vec3{ 0, GRAVITY, 0 };
  }

  vec3 Car::updateWheelForces(float dt, Wheel& wheel, const Terrain& terrain, vec3 connectionPoint, quat wheelRotation, float enginePower, float brakePower)
  {
    vec3 positionGlobal = position + connectionPoint.rotatedBy(rotation);
    vec3 velocityGlobal = velocity + (angularVelocity % connectionPoint).rotatedBy(rotation);
    wheel.carForward = forward();

    return wheel.update(dt, terrain, positionGlobal, wheelRotation, velocityGlobal, enginePower, brakePower).rotatedBy(rotation.inverted());
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

        // suppressing oscillations
        //nForce -= (ptVelocity + nForce / mass * dt).logarithmic() * mass / dt;

        vec3 frictionForce = -ptVelocity.projectedOnPlane(normal) / dt / (pt.sqLength() / momentOfInertia + 1 / mass);
        float frictionForceScalar = frictionForce.length();
        float maxFrictionForce = std::min(nForceScalar, 10000.0f) * config.bodyFriction;
        velocity -= velocity.projectedOnVector(normal) * clamp(penetration * dt * 10, 0.0f, 1.0f);

        if (frictionForceScalar > 0 && frictionForceScalar > maxFrictionForce * maxFrictionForce)
          frictionForce = frictionForce / frictionForceScalar * maxFrictionForce;

        frictionForces.push_back({ frictionForce, ptRotated });
        nForces.push_back({ nForce, ptRotated });
      }
    }

    for (const ForceAndPoint& f : frictionForces)
      applyForceGlobal(f.first / div, f.second);

    for (const ForceAndPoint& f : nForces)
      applyForceGlobal(f.first / div, f.second);
  }

  void Car::updateControl(float dt)
  {
    vec3 thrust = {
      0,
      mass * 20 * float(IsKeyDown(KEY_ENTER)),
      0
    };

    enginePower = config.enginePower * float(IsKeyDown(KEY_HOME) - IsKeyDown(KEY_END));
    //brakePower = config.brakePower * float(IsKeyDown(KEY_END));

    applyForceGlobal(thrust);

    float steeringDirection = float(IsKeyDown(KEY_DELETE) - IsKeyDown(KEY_PAGE_DOWN));
    float maxSteeringAngle = mapRangeClamped(velocity.length(), 0, config.maxSpeed, config.maxSteeringAngle, 0.1f * config.maxSteeringAngle);
    float steeringTarget;

    if (steeringDirection == 0.0f)
    {
      steeringDirection = -sign(steeringAngle);
      steeringTarget = 0.0f;
    }
    else
      steeringTarget = maxSteeringAngle * steeringDirection;

    //float maxSteeringSpeed = mapRangeClamped(velocity.length(), 0, config.maxSpeed, config.maxSteeringSpeed, config.maxSteeringSpeed * 0.5f);

    steeringAngle = moveTo(steeringAngle, steeringTarget, config.maxSteeringSpeed * steeringDirection * dt);
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

    vec3 frontLeftWheelForceGlobal = frontLeftWheelForce.rotatedBy(rotation);
    vec3 frontRightWheelForceGlobal = frontRightWheelForce.rotatedBy(rotation);
    vec3 rearLeftWheelForceGlobal = rearLeftWheelForce.rotatedBy(rotation);
    vec3 rearRightWheelForceGlobal = rearRightWheelForce.rotatedBy(rotation);

    drawVector(frontLeftWheelConnectionPointGlobal, 0.5f * frontLeftWheelForceGlobal.logarithmic(), RED);
    drawVector(frontRightWheelConnectionPointGlobal, 0.5f * frontRightWheelForceGlobal.logarithmic(), RED);
    drawVector(rearLeftWheelConnectionPointGlobal, 0.5f * rearLeftWheelForceGlobal.logarithmic(), RED);
    drawVector(rearRightWheelConnectionPointGlobal, 0.5f * rearRightWheelForceGlobal.logarithmic(), RED);
  }
}

