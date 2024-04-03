#include "pch.h"
#include "Wheel.h"
#include "Helpers.h"

namespace game
{
  Wheel::Wheel(const Config& config, bool isFrontWheel, const Terrain& terrain, vec3 connectionPoint, const char* debugName) :
    config(config),
    wheelConfig(isFrontWheel ? config.physics.frontWheels : config.physics.rearWheels),
    terrain(terrain),
    connectionPoint(connectionPoint),
    debugName(debugName)
  {
    momentOfInertia = 0.5f * wheelConfig.mass * sqr(wheelConfig.radius);
  }

  void Wheel::update(float dt, const DynamicObject& parent, float steeringAngle, float sharedMass, float enginePower, bool handBreaked)
  {
    lastPosition = position;
    lastRotation = rotation;

    vec3 globalConnectionPoint = connectionPoint.rotatedBy(parent.rotation);
    position = parent.position + globalConnectionPoint;
    rotation = parent.rotation * quat::fromYAngle(steeringAngle);
    velocity = parent.velocity + parent.angularVelocity.rotatedBy(rotation) % globalConnectionPoint;
    suspecsionForce = vec3::zero;

    float springForce = sqr(suspensionOffset) * wheelConfig.suspensionStiffness;
    float springForceSigned = springForce * sign(-suspensionOffset);
    suspensionSpeed += springForceSigned / wheelConfig.mass * dt;
    float dampingForce = -suspensionSpeed * wheelConfig.mass / dt * wheelConfig.suspensionDamping;
    suspensionSpeed += dampingForce / wheelConfig.mass * dt;
    suspensionOffset += suspensionSpeed * dt;

    wheelRotationSpeed += enginePower / momentOfInertia * dt;
    wheelRotationSpeed = moveTo(wheelRotationSpeed, 0, std::max(wheelRotationSpeed * wheelConfig.rollingFriction, 0.01f));
    float maxRPS = float(!handBreaked) * (20 + 4 * velocity.length());
    wheelRotationSpeed = clamp(wheelRotationSpeed, -maxRPS, maxRPS);

    vec3 normal;
    float terrainY = terrain.getHeight(position.x, position.z, &normal);
    float bottomY = position.y + suspensionOffset - wheelConfig.radius;
    float penetration = terrainY - bottomY;
    isGrounded = penetration > 0;

    vec3 lastFrictionForce = frictionForce;
    frictionForce = vec3::zero;

    if (isGrounded)
    {
      vec3 frictionUp = normal;
      vec3 frictionForward = vec3::forward.rotatedBy(rotation).projectedOnPlane(normal).normalized();
      vec3 frictionLeft = frictionUp % frictionForward;

      suspensionOffset += penetration;
      suspensionSpeed = -velocity * vec3::up;

      nForce = springForce * normal;
      suspecsionForce += nForce;

      float parentNormalSpeed = velocity * normal;

      if (parentNormalSpeed < 0)
      {
        vec3 dampingForce = -parentNormalSpeed * sharedMass / dt * wheelConfig.suspensionDamping * normal;
        suspecsionForce += dampingForce;
      }

      float frictionSpeed = (velocity.projectedOnPlane(normal) - wheelRotationSpeed * wheelConfig.radius * frictionForward).length();
      float frictionKoef = wheelConfig.tireFriction + mapRangeClamped(frictionSpeed, 5, 30, 0.2f, -0.1f);
      float maxFrictionForce = std::min(springForce * frictionKoef, sharedMass * 100);
      float gravity = config.physics.gravity;

      vec3 gravityVelocity = handBreaked ?
        vec3 { 0, -gravity * dt, 0 }.projectedOnPlane(normal) :
        vec3 { 0, -gravity * dt, 0 }.projectedOnVector(frictionLeft);

      vec3 frictionVelocityForecast = velocity + gravityVelocity - wheelRotationSpeed * wheelConfig.radius * frictionForward;
      frictionForce = -frictionVelocityForecast.projectedOnPlane(normal) * sharedMass / dt;

      if (frictionForce.sqLength() > sqr(maxFrictionForce))
        frictionForce = frictionForce.normalized() * maxFrictionForce;

      frictionForce = 0.5 * (frictionForce + lastFrictionForce);

      suspecsionForce += frictionForce;

      float targetWheelRotationSpeed = float(!handBreaked) * velocity * frictionForward / wheelConfig.radius;
      float step = (frictionForce * frictionForward) * wheelConfig.radius / momentOfInertia * dt;
      wheelRotationSpeed = moveTo(wheelRotationSpeed, targetWheelRotationSpeed, step);

      frictionVelocity = velocity.projectedOnPlane(normal) - wheelRotationSpeed * wheelConfig.radius * frictionForward;
    }

    suspensionOffset = clamp(suspensionOffset, -wheelConfig.maxSuspensionOffset, wheelConfig.maxSuspensionOffset);
    wheelRotation.rotateByXAngle(wheelRotationSpeed * dt);
    
    position.y += suspensionOffset;
    rotation = rotation * wheelRotation;
  }

  void Wheel::reset()
  {
    lastPosition = vec3::zero;
    lastRotation = quat::identity;
    position = vec3::zero;
    rotation = quat::identity;
    velocity = vec3::zero;
    wheelRotation = quat::identity;
    wheelRotationSpeed = 0;
    suspensionOffset = 0;
    suspensionSpeed = 0;
    nForce = vec3::zero;
    frictionForce = vec3::zero;
    frictionVelocity = vec3::zero;
  }

}