#include "core.h"
#include "Car.h"
#include "Helpers.h"
#include "PlayerControl.h"
#include "Scene.h"

namespace game
{
  Car::Car(uint64_t guid, const Config& config, const Scene& scene) :
    guid(guid),
    config(config),
    carConfig(config.physics.car),
    gravity(config.physics.gravity),
    health(config.physics.car.maxHealth),
    gun(config.physics.gun, scene, config.physics.car.connectionPoints.weapon.gun),
    cannon(config.physics.cannon, scene, config.physics.car.connectionPoints.weapon.cannon),
    frontLeftWheel(config, true, scene.terrain, config.physics.car.connectionPoints.wheels.frontLeft, "FrontLeftWheel"),
    frontRightWheel(config, true, scene.terrain, config.physics.car.connectionPoints.wheels.frontRight, "FrontRightWheel"),
    rearLeftWheel(config, false, scene.terrain, config.physics.car.connectionPoints.wheels.rearLeft, "RearLeftWheel"),
    rearRightWheel(config, false, scene.terrain, config.physics.car.connectionPoints.wheels.rearRight, "RearRightWheel"),
    scene(scene)
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
    gunRecoilForce = vec3::zero;
    cannonRecoilForce = vec3::zero;
    hitForce = vec3::zero;
    hitMoment = vec3::zero;
    enginePower = 0;
    handBreaked = false;
    steeringAngle = 0.0f;
    frontLeftWheel.reset(*this);
    frontRightWheel.reset(*this);
    rearLeftWheel.reset(*this);
    rearRightWheel.reset(*this);
    gun.reset(*this);
    cannon.reset(*this);
  }

  bool Car::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    vec3 closestHitPosition = vec3::zero;
    vec3 closestHitNormal = vec3::zero;
    float closestHitDistance = FLT_MAX;

    vec3 currentHitPosition = vec3::zero;
    vec3 currentHitNormal = vec3::zero;
    float currentHitDistance = FLT_MAX;

    const Sphere(&carSpheres)[4] = config.collisionGeometries.carSpheres;

    for (const Sphere sphere : carSpheres)
    {
      Sphere worldSphere = {
        position + sphere.center.rotatedBy(rotation),
        sphere.radius
      };

      if (worldSphere.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentHitNormal, &currentHitDistance) && currentHitDistance < closestHitDistance)
      {
        closestHitPosition = currentHitPosition;
        closestHitNormal = currentHitNormal;
        closestHitDistance = currentHitDistance;
      }
    }

    if (closestHitDistance < distance)
    {
      if (hitPosition)
        *hitPosition = closestHitPosition;

      if (hitNormal)
        *hitNormal = closestHitNormal;

      if (hitDistance)
        *hitDistance = closestHitDistance;

      return true;
    }

    return false;
  }

  void Car::update(float dt)
  {
    resetForces();
    applyGlobalForceAtCenterOfMass({ 0, -gravity * mass, 0 });
    applyGlobalForceAtCenterOfMass({ 0, 2 * gravity * mass * thrustAxis, 0 });

    float aerodinamicForce = carConfig.aerodynamicKoef * velocity.sqLength();
    applyGlobalForceAtCenterOfMass(-aerodinamicForce * up());

    if (health > 0)
    {
      vec3 alignmentMoment = getAutoAlignmentMoment(dt);
      applyMoment(alignmentMoment);
    }

    vec3 airFrictionMoment = -angularVelocity / dt * momentOfInertia * 0.001f;
    applyMoment(airFrictionMoment);

    updateEngine(dt);
    updateSteering(dt);

    applyGlobalForceAtLocalPoint(frontLeftWheel.suspensionForce, frontLeftWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(frontRightWheel.suspensionForce, frontRightWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(rearLeftWheel.suspensionForce, rearLeftWheel.connectionPoint);
    applyGlobalForceAtLocalPoint(rearRightWheel.suspensionForce, rearRightWheel.connectionPoint);

    applyGlobalForceAtLocalPoint(gunRecoilForce, gun.connectionPoint + vec3(0, gun.config.barrelElevation, 0));
    applyGlobalForceAtLocalPoint(cannonRecoilForce, cannon.connectionPoint + vec3(0, cannon.config.barrelElevation, 0));

    applyGlobalForceAtCenterOfMass(hitForce);
    applyMoment(hitMoment);

    gunRecoilForce = moveToRelative(gunRecoilForce, vec3::zero, 20 * dt);
    cannonRecoilForce = moveToRelative(cannonRecoilForce, vec3::zero, 20 * dt);
    hitForce = moveToRelative(hitForce, vec3::zero, 10 * dt);
    hitMoment = moveToRelative(hitMoment, vec3::zero, 10 * dt);

    updateCollisions(dt);

    PhysicalObject::update(dt);

    updateWheels(dt);

    if (aliveState == Alive)
    {
      gun.update(dt, *this);
      cannon.update(dt, *this);
    }
    else
    {
      gun.updateLocked(dt, *this);
      cannon.updateLocked(dt, *this);
    }

    updateAliveStateTimeout(dt);
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

    const Sphere(&carSpheres)[4] = config.collisionGeometries.carSpheres;
    constexpr int MAX_SPHERES = (Scene::MAX_CARS - 1) * sizeof(carSpheres) / sizeof(carSpheres[0]);
    std::vector<Sphere> worldCarSpheres;
    worldCarSpheres.reserve(MAX_SPHERES);

    // TODO: Consider moving update collisions to Scene to reduce number of pairs by half
    if (aliveState != Car::Countdown && aliveState != Car::Hidden)
      for (int i = 0; i < scene.cars.capacity(); i++)
        if (scene.cars.exists(i) && &scene.cars[i] != this && scene.cars[i].aliveState != Car::Countdown && scene.cars[i].aliveState != Car::Hidden)
        {
          const Car& car = scene.cars[i];

          for (const Sphere& s : carSpheres)
            worldCarSpheres.push_back({
              .center = car.position + s.center.rotatedBy(car.rotation),
              .radius = s.radius
            });
        }

    for (Sphere sphere : collisionSpheres)
    {
      Sphere worldSphere = {
        position + sphere.center.rotatedBy(rotation),
        sphere.radius
      };

      vec3 collisionPoint {};
      vec3 collisionNormal {};
      float penetration = 0;

      if (scene.terrain.collideSphereWithObjects(worldSphere, &collisionPoint, &collisionNormal, &penetration))
      {
        vec3 carPoint = collisionPoint - collisionNormal * penetration - position;
        hits.push_back({ carPoint, collisionNormal, penetration });
      }

      for (const Sphere& s : worldCarSpheres)
        if (s.collideWith(worldSphere, &collisionPoint, &collisionNormal, &penetration))
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

      float terrainY = scene.terrain.getHeight(carPointGlobal.x, carPointGlobal.z, &normal);
      float penetration = std::max(terrainY - carPointGlobal.y, 0.0f);

      if (penetration > 0)
        hits.push_back({ carPoint, normal, penetration });
    }

    typedef std::pair<vec3, vec3> ForceAndPoint;
    std::vector<ForceAndPoint> frictionForces;
    std::vector<ForceAndPoint> nForces;
    float div = 0;

    for (auto [point, normal, penetration] : hits)
    {
      vec3 ptLocal = point.rotatedBy(rotation.inverted());
      vec3 ptRotationfVelocityGlobal = (angularVelocity % ptLocal).rotatedBy(rotation);
      vec3 ptVelocityGlobal = velocity + ptRotationfVelocityGlobal;

      div += 1.0f;
      float nForceScalar = 20 * mass / dt * penetration * penetration;
      nForceScalar = std::min(nForceScalar, 50000.0f);
      vec3 nForce = normal * nForceScalar;

      vec3 frictionForce = -ptVelocityGlobal.projectedOnPlane(normal) / dt / (ptLocal.sqLength() / momentOfInertia + 1 / mass);
      float frictionForceScalar = frictionForce.length();
      float maxFrictionForce = std::min(nForceScalar, 200000.0f) * carConfig.bodyFriction;
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
    handBreaked = playerControl.handBrake;
    gunFiring = playerControl.primaryFire;
    cannonFiring = playerControl.secondaryFire;
    accelerationAxis = playerControl.accelerationAxis;
    steeringAxis = playerControl.steeringAxis;
    thrustAxis = playerControl.thrustAxis;
    gun.expectedTarget = playerControl.target;
    cannon.expectedTarget = playerControl.target;
  }

  void Car::blockControl()
  {
    handBreaked = true;
    gunFiring = false;
    cannonFiring = false;
    accelerationAxis = 0;
    steeringAxis = FLT_MAX;
    thrustAxis = 0;
    gun.expectedTarget = vec3::zero;
    cannon.expectedTarget = vec3::zero;
  }

  PlayerState Car::getState() const
  {
    return {
      .position = position,
      .rotation = rotation,
      .velocity = velocity,
      .angularVelocity = angularVelocity,
      .steeringAngle = steeringAngle,
      .health = health,
      .aliveStateTimeout = aliveStateTimeout,
      .frontLeftWheelState = frontLeftWheel.getState(),
      .frontRightWheelState = frontRightWheel.getState(),
      .rearLeftWheelState = rearLeftWheel.getState(),
      .rearRightWheelState = rearRightWheel.getState(),
      .gunState = gun.getState(),
      .cannonState = cannon.getState(),
    };
  }

  void Car::syncState(const PlayerState& playerState, float syncFactor)
  {
    bool isRespawn = playerState.velocity.isZero() && !(playerState.position - position).isAlmostZero(0.01f);

    if (isRespawn)
      resetToPosition(playerState.position, playerState.rotation);

    float relativeSyncFactor = isRespawn ? 1.0f : 0.1f * syncFactor;
    syncFactor = isRespawn ? 1.0f : syncFactor;

    position = moveToRelative(position, playerState.position, relativeSyncFactor);
    velocity = vec3::lerp(velocity, playerState.velocity, syncFactor);
    rotation = quat::slerp(rotation, playerState.rotation, syncFactor);
    angularVelocity = vec3::lerp(angularVelocity, playerState.angularVelocity, syncFactor);
    steeringAngle = lerp(steeringAngle, playerState.steeringAngle, syncFactor);
    health = playerState.health;
    aliveStateTimeout = playerState.aliveStateTimeout;
    updateAliveState();

    const SteeringAngles steeringAngles = calcSteeringAngles();
    frontLeftWheel.syncState(playerState.frontLeftWheelState, syncFactor, steeringAngles.frontLeft, *this);
    frontRightWheel.syncState(playerState.frontRightWheelState, syncFactor, steeringAngles.frontRight, *this);
    rearLeftWheel.syncState(playerState.rearLeftWheelState, syncFactor, steeringAngles.rearLeft, *this);
    rearRightWheel.syncState(playerState.rearRightWheelState, syncFactor, steeringAngles.rearRight, *this);

    gun.syncState(playerState.gunState, syncFactor, *this);
    cannon.syncState(playerState.cannonState, syncFactor, *this);
  }

  void Car::updateEngine(float dt)
  {
    float signedForwardSpeedSqr = sqrSigned(velocity * forward());
    float maxForwardEnginePower = mapRangeClamped(signedForwardSpeedSqr, 0, sqr(carConfig.maxForwardSpeed), carConfig.enginePower, 0);
    float maxBackwardEnginePower = mapRangeClamped(signedForwardSpeedSqr, -sqr(carConfig.maxBackwardSpeed), 0, 0, carConfig.enginePower);

    float expectedPower = float(accelerationAxis > 0) * maxForwardEnginePower - (accelerationAxis < 0) * maxBackwardEnginePower;

    if (sign(accelerationAxis) == -sign(enginePower) || (accelerationAxis == 0 && handBreaked))
      enginePower = 0;
    else
      enginePower = moveTo(enginePower, expectedPower, (accelerationAxis == 0 ? 3 : 1) * carConfig.enginePower * dt);
  }

  void Car::updateSteering(float dt)
  {
    if (steeringAxis == FLT_MAX)
      return;

    float maxSteeringAngle = mapRangeClamped(velocity.length(), 0.25f * carConfig.maxForwardSpeed, carConfig.maxForwardSpeed, carConfig.maxSteeringAngle, carConfig.minSteeringAngle);
    //float maxSteeringSpeed = mapRangeClamped(velocity.length(), 0, carConfig.maxSpeed, carConfig.maxSteeringSpeed, carConfig.maxSteeringSpeed * 0.5f);

    if (steeringAxis == 0.0f)
      steeringAngle = moveTo(steeringAngle, 0, carConfig.maxSteeringSpeed * dt);
    else
    {
      float steeringTarget = maxSteeringAngle * sign(steeringAxis);
      float steeringSpeed = carConfig.maxSteeringSpeed * steeringAxis;
      steeringAngle = moveTo(steeringAngle, steeringTarget, steeringSpeed * dt);
    }
  }

  void Car::updateWheels(float dt)
  {
    int frontContactsCount = int(frontLeftWheel.isGrounded + frontRightWheel.isGrounded);
    int rearContactsCount = int(rearLeftWheel.isGrounded + rearRightWheel.isGrounded);

    const auto& wheelsConnectionPoints = config.physics.car.connectionPoints.wheels;
    const float frontWeigthDistribution = clamp(fabsf(wheelsConnectionPoints.rearLeft.z) / (wheelsConnectionPoints.frontLeft.z - wheelsConnectionPoints.rearLeft.z), 0.0f, 1.0f);
    const float rearWeigthDistribution = 1 - frontWeigthDistribution;

    float frontMass = rearContactsCount ? mass * frontWeigthDistribution : mass;
    float frontSharedMass = frontContactsCount ? frontMass / frontContactsCount : 0;

    float rearMass = frontContactsCount ? mass * rearWeigthDistribution : mass;
    float rearSharedMass = rearContactsCount ? rearMass / rearContactsCount : 0;

    float powerDistributionRatio = config.physics.car.frontRearPowerDistributionRatio;
    float frontPower = (1.0f - powerDistributionRatio) * enginePower;
    float rearPower = powerDistributionRatio * enginePower;

    const SteeringAngles steeringAngles = calcSteeringAngles();
    frontLeftWheel.update(dt, *this, steeringAngles.frontLeft, frontSharedMass, frontPower, handBreaked);
    frontRightWheel.update(dt, *this, steeringAngles.frontRight, frontSharedMass, frontPower, handBreaked);
    rearLeftWheel.update(dt, *this, steeringAngles.rearLeft, rearSharedMass, rearPower, handBreaked);
    rearRightWheel.update(dt, *this, steeringAngles.rearRight, rearSharedMass, rearPower, handBreaked);
  }

  Car::SteeringAngles Car::calcSteeringAngles() const
  {
    static const vec3 turnRadiusVector = carConfig.connectionPoints.wheels.frontLeft -
      0.5f * (carConfig.connectionPoints.wheels.rearLeft + carConfig.connectionPoints.wheels.rearRight);

    static const float steeringMaxCorrectionAngle = turnRadiusVector.getYAngle();

    float correctionAngle = fabs(mapRangeClamped(steeringAngle, -PI / 2, PI / 2, -steeringMaxCorrectionAngle, steeringMaxCorrectionAngle));

    return {
      .frontLeft = steeringAngle + correctionAngle,
      .frontRight = steeringAngle - correctionAngle,
      .rearLeft = 0,
      .rearRight = 0
    };
  }

  void Car::updateAliveStateTimeout(float dt)
  {
    if (aliveState != Alive && aliveState != Hidden)
    {
      aliveStateTimeout = std::max(0.0f, aliveStateTimeout - dt);
      updateAliveState();
    }
  }

  void Car::updateAliveState()
  {
    float threshold = 0;

    for (auto [state, duration] : aliveStatesOrdered)
    {
      if (aliveStateTimeout <= threshold + duration)
      {
        aliveState = state;
        return;
      }

      threshold += duration;
    }

    aliveState = AliveState::Unknown;
  }

  float Car::getAliveStateTimeout() const
  {
    float threshold = 0;

    for (auto [state, duration] : aliveStatesOrdered)
    {
      if (aliveStateTimeout <= threshold + duration)
        return aliveStateTimeout - threshold;

      threshold += duration;
    }

    return 0.0f;
  }

  float Car::getAliveStateTimeoutProgress() const
  {
    float threshold = 0;

    for (auto [state, duration] : aliveStatesOrdered)
    {
      if (aliveStateTimeout <= threshold + duration)
        return duration > EPSILON ? (aliveStateTimeout - threshold) / duration : 0.0f;

      threshold += duration;
    }

    return 0.0f;
  }

  void Car::switchToAliveState(AliveState newState)
  {
    float timeout = 0;

    for (auto [state, duration] : aliveStatesOrdered)
    {
      timeout += duration;

      if (state == newState)
      {
        aliveStateTimeout = timeout;
        aliveState = state;

        return;
      }
    }
  }

  bool Car::isLocal() const
  {
    return scene.localPlayerGuid == guid;
  }

  bool Car::isRemote() const
  {
    return !isLocal();
  }

}

