#include "pch.h"
#include "Wheel.h"
#include "Helpers.h"

namespace game
{
  void Wheel::init(const Config::Physics::Wheels& config, const Model& model, const char* debugName)
  {
    this->config = config;
    this->debugName = debugName;
    Renderable::init(model);
  }

  vec3 Wheel::update(float dt, const Terrain& terrain, vec3 parentPosition, quat parentRotation, vec3 parentVelocity, float enginePower, float brakePower)
  {
    position = parentPosition;
    rotation = parentRotation;

    float MAX_PENETRATION = 0.1f;

    {
      float springForce = -suspensionOffset * config.suspensionStiffness;
      suspensionSpeed += springForce / config.mass * dt;
      float dampingForce = -suspensionSpeed * config.suspensionDamping;
      float maxDampingForce = fabsf(suspensionSpeed) * config.mass / dt;
      dampingForce = clamp(dampingForce, -maxDampingForce, maxDampingForce);
      suspensionSpeed += dampingForce / config.mass * dt;
      //suspensionSpeed = moveTo(suspensionSpeed, 0, 0.1f * suspensionSpeed);

      suspensionOffset += suspensionSpeed * dt;
    }

    vec3 normal;
    float terrainY = terrain.getHeight2(position.x, position.z, &normal);
    float bottomY = position.y + suspensionOffset - config.radius;
    float penetration = std::max(terrainY - bottomY, 0.0f) / MAX_PENETRATION;

    if (penetration > 0)
    {
      suspensionOffset += terrainY - bottomY;
      suspensionSpeed = -parentVelocity * vec3::up;
    }

    //position.y = parentPosition.y + suspensionOffset;


    //suspensionOffset = clamp(suspensionOffset, -frontWheelsMaxSuspensionOffset, frontWheelsMaxSuspensionOffset);

    wheelRotation = quat::fromEuler(0, 0, wheelAngularVelocity * dt) * wheelRotation;

    vec3 force = vec3::zero;
    isGrounded = penetration > 0;

    if (penetration > 0)
    {
      wheelAngularVelocity = parentVelocity.rotatedBy(rotation.inverted()).projectedOnVector(vec3::forward).z / config.radius;

      float nForceScalar = sqr(suspensionOffset) * config.suspensionStiffness;
      nForce = nForceScalar * normal;
      force += nForce;

      vec3 dampingForce = vec3{ 0, -parentVelocity.y, 0 } * config.suspensionDamping;
      force += dampingForce;

      vec3 wheelForward = vec3::forward.rotatedBy(rotation);
      vec3 wheelLeft = vec3::left.rotatedBy(rotation);
      vec3 velocityNormalized = parentVelocity;// .logarithmic();

      frictionVelocity = parentVelocity.projectedOnVector(wheelLeft).projectedOnPlane(normal);

      vec3 wheelSideVelocityFactor = parentVelocity.logarithmic().projectedOnVector(wheelLeft).projectedOnPlane(normal);
      vec3 wheelForwardVelocityFactor = parentVelocity.projectedOnVector(wheelForward).projectedOnPlane(normal);
      frictionForce = vec3::zero;
      frictionForce = -wheelSideVelocityFactor * 1000.0f / dt;
      //frictionForce += -wheelForwardVelocityFactor * nForceScalar * config.rollingFriction;
      frictionForce += (wheelLeft % normal) * enginePower;
      frictionForce += -parentVelocity.logarithmic().projectedOnVector(wheelLeft % normal) * brakePower;

      float maxFrictionForce = std::min(nForceScalar, 2000.0f) * config.tireFriction * 10;

      if (frictionForce.sqLength() > sqr(maxFrictionForce))
        frictionForce = frictionForce.normalized() * maxFrictionForce;

      force += frictionForce;

      //vec3 engineForce = (wheelLeft % normal).normalized() * enginePower;
      //force += engineForce;

      //vec3 brakeForce = -parentVelocity.logarithmic().projectedOnVector(wheelLeft % normal) * brakePower;
      //force += brakeForce;

      if (debugName == "FrontLeftWheel")
      {
        int i = 0;
      }
    }

    suspensionOffset = clamp(suspensionOffset, -config.maxSuspensionOffset, config.maxSuspensionOffset);

    return force;
  }

  void Wheel::draw(bool drawWires)
  {
    //if (isGrounded)
    {
      vec3 bottom = position + vec3{ 0, -config.radius + 0.2f, 0 };
      DrawSphere(bottom, 0.3f, ORANGE);
      drawVector(bottom, frictionVelocity, GREEN);
      drawVector(bottom, frictionForce.logarithmic(), ORANGE);
    }

    Matrix transform = MatrixMultiply(QuaternionToMatrix(rotation * wheelRotation), MatrixTranslate(position.x, position.y + suspensionOffset, position.z));
    Renderable::draw(transform, drawWires);
  }
  void Wheel::reset()
  {
    position = vec3::zero;
    rotation = quat::identity;
    suspensionOffset = 0;
    suspensionSpeed = 0;
    nForce = vec3::zero;
    frictionForce = vec3::zero;
    frictionVelocity = vec3::zero;
  }
}