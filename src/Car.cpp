#include "core.h"
#include "Car.h"
#include "Helpers.h"
#include "PlayerControl.h"

namespace game
{
  Car::Car(uint64_t guid, const Config& config, const Terrain& terrain) :
    guid(guid),
    config(config),
    carConfig(config.physics.car),
    gravity(config.physics.gravity),
    gun(config.physics.gun, terrain, config.physics.car.connectionPoints.weapon.gun, 1),
    cannon(config.physics.cannon, terrain, config.physics.car.connectionPoints.weapon.cannon, 2),
    frontLeftWheel(config, true, terrain, config.physics.car.connectionPoints.wheels.frontLeft, "FrontLeftWheel"),
    frontRightWheel(config, true, terrain, config.physics.car.connectionPoints.wheels.frontRight, "FrontRightWheel"),
    rearLeftWheel(config, false, terrain, config.physics.car.connectionPoints.wheels.rearLeft, "RearLeftWheel"),
    rearRightWheel(config, false, terrain, config.physics.car.connectionPoints.wheels.rearRight, "RearRightWheel"),
    terrain(terrain)
  {
    mass = config.physics.car.mass;
    vec3 size = { 2.04f, 2.32f, 4.56f };
    float radius = (size.x + size.y + size.z) / 6;
    momentOfInertia = 0.5f * mass * sqr(radius);
  }

  void Car::resetToPosition(vec3 position, quat rotation)
  {
    lastPosition = position;
    lastRotation = rotation;
    this->position = position;
    this->rotation = rotation;
    velocity = vec3::zero;
    angularVelocity = vec3::zero;
    force = vec3::zero;
    moment = vec3::zero;
    enginePower = 0;
    handBreaked = false;
    steeringAngle = 0.0f;
    frontLeftWheel.reset();
    frontRightWheel.reset();
    rearLeftWheel.reset();
    rearRightWheel.reset();
  }

  bool Car::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    return false;
  }

  void Car::update(float dt)
  {
    resetForces();
    applyGlobalForceAtCenterOfMass({ 0, -gravity * mass, 0 });

    float aerodinamicForce = carConfig.aerodynamicKoef * velocity.sqLength();
    applyGlobalForceAtCenterOfMass(-aerodinamicForce * up());

    applyGlobalForceAtCenterOfMass({ 0, verticalTrust, 0 });

    vec3 alignmentMoment = getAutoAlignmentMoment(dt);
    applyMoment(alignmentMoment);

    vec3 airFrictionMoment = -angularVelocity / dt * momentOfInertia * 0.001f;
    applyMoment(airFrictionMoment);

    updateEngine(dt);
    updateSteering(dt);

    applyGlobalForceAtLocalPoint(frontLeftWheel.suspecsionForce, frontLeftWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(frontRightWheel.suspecsionForce, frontRightWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(rearLeftWheel.suspecsionForce, rearLeftWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(rearRightWheel.suspecsionForce, rearRightWheel.connectionPoint);

    updateCollisions(dt);

    PhysicalObject::update(dt);

    updateWheels(dt);
    gun.update(dt, *this);
    cannon.update(dt, *this);
  }

  void Car::updateCollisions(float dt)
  {
    const vec3(&collisionPoints)[10] = config.collisionGeometries.carPoints;
    const Sphere(&collisionSpheres)[4] = config.collisionGeometries.carSpheres;

    if (IsKeyDown(KEY_F))
    {
      applyMoment({ 0, 0, -10000 });
    }

    typedef std::tuple<vec3, vec3, float> Hit;
    std::vector<Hit> hits;

    typedef std::pair<vec3, vec3> ForceAndPoint;
    std::vector<ForceAndPoint> frictionForces;
    std::vector<ForceAndPoint> nForces;
    float div = 0;

    for (Sphere sphere : collisionSpheres)
    {
      Sphere worldSphere = {
        position + sphere.center.rotatedBy(rotation),
        sphere.radius
      };

      vec3 collisionPoint {};
      vec3 collisionNormal {};
      float penetration = 0;

      if (terrain.collideSphereWithObjects(worldSphere, &collisionPoint, &collisionNormal, &penetration))
      {
        vec3 carPoint = collisionPoint - collisionNormal * penetration - position;
        hits.push_back({ carPoint, collisionNormal, penetration });
      }
    }

    for (vec3 pt : collisionPoints)
    {
      vec3 carPoint = pt.rotatedBy(rotation);
      vec3 carPointGlobal = carPoint + position;
      vec3 normal;

      const float MAX_PENETRATION = 0.1f;

      float terrainY = terrain.getHeight(carPointGlobal.x, carPointGlobal.z, &normal);
      float penetration = std::max(terrainY - carPointGlobal.y, 0.0f);

      if (penetration > 0)
        hits.push_back({ carPoint, normal, penetration });
    }

    for (auto [point, normal, penetration] : hits)
    {
      vec3 ptLocal = point.rotatedBy(rotation.inverted());
      const float MAX_PENETRATION = 0.1f;

      vec3 ptRotationfVelocityGlobal = (angularVelocity % ptLocal).rotatedBy(rotation);
      vec3 ptVelocityGlobal = velocity + ptRotationfVelocityGlobal;

      div += 1.0f;
      float nForceScalar = 20 * mass / dt * penetration * penetration;
      nForceScalar = std::min(nForceScalar, 500000.0f);
      vec3 nForce = normal * nForceScalar;

      vec3 frictionForce = -ptVelocityGlobal.projectedOnPlane(normal) / dt / (ptLocal.sqLength() / momentOfInertia + 1 / mass);
      float frictionForceScalar = frictionForce.length();
      float maxFrictionForce = std::min(nForceScalar, 2000000.0f) * carConfig.bodyFriction;
      velocity -= velocity.projectedOnVector(normal) * clamp(penetration * penetration * dt * 400, 0.0f, 1.0f);

      if (frictionForceScalar > 0 && frictionForceScalar > maxFrictionForce)
        frictionForce = frictionForce / frictionForceScalar * maxFrictionForce;

      frictionForces.push_back({ frictionForce, point });
      nForces.push_back({ nForce, point });
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

  void Car::updateControl(const PlayerControl& playerControl)
  {
    verticalTrust = mass * 20 * playerControl.thrustAxis;
    handBreaked = playerControl.handBrake;
    gunFiring = playerControl.primaryFire;
    cannonFiring = playerControl.secondaryFire;
    enginePowerDirection = playerControl.accelerationAxis;
    steeringDirection = playerControl.steeringAxis;
    gun.expectedTarget = playerControl.target;
    cannon.expectedTarget = playerControl.target;
  }

  void Car::syncState(const PlayerState& playerState, float syncFactor)
  {
    position = vec3::lerp(position, playerState.position, syncFactor);
    velocity = vec3::lerp(velocity, playerState.velocity, syncFactor);
    rotation = quat::slerp(rotation, playerState.rotation, syncFactor);
    angularVelocity = vec3::lerp(angularVelocity, playerState.angularVelocity, syncFactor);
    gun.syncState(playerState.gunYaw, playerState.gunPitch, syncFactor);
    cannon.syncState(playerState.cannonYaw, playerState.cannonPitch, syncFactor);
  }

  void Car::updateEngine(float dt)
  {
    float ep = (sign(enginePowerDirection) == sign(enginePower)) ? carConfig.enginePower : carConfig.brakePower;
    float step = ep * dt;
    float maxForwardEnginePower = mapRangeClamped(velocity * forward(), 0, carConfig.maxSpeed, ep, 0);
    float maxBackwardEnginePower = mapRangeClamped(velocity * forward(), -carConfig.maxSpeed, 0, 0, ep);
    enginePower = moveTo(enginePower, maxForwardEnginePower * float(enginePowerDirection > 0) - maxBackwardEnginePower * float(enginePowerDirection < 0), step);
  }

  void Car::updateSteering(float dt)
  {
    float maxSteeringAngle = mapRangeClamped(velocity * forward(), 0.1f * carConfig.maxSpeed, 0.5f * carConfig.maxSpeed, carConfig.maxSteeringAngle, carConfig.minSteeringAngle);
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

  void Car::updateWheels(float dt)
  {
    float maxCorrectionAngle = (frontLeftWheel.connectionPoint - 0.5f * (rearLeftWheel.connectionPoint + rearRightWheel.connectionPoint)).getYAngle();
    float correctionAngle = fabs(mapRangeClamped(steeringAngle, -PI / 2, PI / 2, -maxCorrectionAngle, maxCorrectionAngle));

    // TODO: Move to config the power coefficients for front and rear wheels
    float frontPower = 1.0f * enginePower;
    float rearPower = 1.0f * enginePower;
    float contactsCount = std::max(1.0f, float(frontLeftWheel.isGrounded + frontRightWheel.isGrounded + rearLeftWheel.isGrounded + rearRightWheel.isGrounded));
    float sharedMass = mass / contactsCount;
    float frontLeftSteeringAngle = steeringAngle + correctionAngle;
    float frontRightSteeringAngle = steeringAngle - correctionAngle;

    // TODO: Implement in config the mass sharing coefficient between front and rear wheels
    frontLeftWheel.update(dt, *this, frontLeftSteeringAngle, sharedMass, frontPower, handBreaked);
    frontRightWheel.update(dt, *this, frontRightSteeringAngle, sharedMass, frontPower, handBreaked);
    rearLeftWheel.update(dt, *this, 0, sharedMass, rearPower, handBreaked);
    rearRightWheel.update(dt, *this, 0, sharedMass, rearPower, handBreaked);
  }

}

