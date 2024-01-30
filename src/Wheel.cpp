#include "pch.h"
#include "Wheel.h"
#include "Helpers.h"

namespace game
{
  void Wheel::init(const Config::Physics::Wheels& config, const Model& model, const char* debugName)
  {
    this->debugName = debugName;
    wheelConfig = config;
    Renderable::init(model);
  }

  vec3 Wheel::update(float dt, const Terrain& terrain, vec3 parentPosition, quat parentRotation, vec3 parentVelocity, float enginePower, float brakePower)
  {
    position = parentPosition;
    rotation = parentRotation;

    float MAX_PENETRATION = 0.1f;

    {
      float springForce = -suspensionOffset * wheelConfig.suspensionStiffness;
      suspensionSpeed += springForce / wheelConfig.mass * dt;
      float dampingForce = -suspensionSpeed * wheelConfig.mass / dt * 0.3f;
      //float maxDampingForce = fabsf(suspensionSpeed) * carConfig.mass / dt;
      //dampingForce = clamp(dampingForce, -maxDampingForce, maxDampingForce);
      suspensionSpeed += dampingForce / wheelConfig.mass * dt;
      //suspensionSpeed = moveTo(suspensionSpeed, 0, 0.1f * suspensionSpeed);

      suspensionOffset += suspensionSpeed * dt;
    }

    vec3 normal;
    float terrainY = terrain.getHeight2(position.x, position.z, &normal);
    float bottomY = position.y + suspensionOffset - wheelConfig.radius;
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
      wheelAngularVelocity = parentVelocity.rotatedBy(rotation.inverted()).projectedOnVector(vec3::forward).z / wheelConfig.radius;

      vec3 suspensionUp = vec3::up.rotatedBy(rotation);
      vec3 suspensionForward = vec3::forward.rotatedBy(rotation);
      vec3 suspensionLeft = vec3::left.rotatedBy(rotation);

      float nForceScalar = sqr(suspensionOffset) * wheelConfig.suspensionStiffness;
      nForce = nForceScalar * normal;
      force += nForce;

      if (parentVelocity * suspensionUp < 0)
      {
        vec3 dampingForce = -parentVelocity.projectedOnVector(suspensionUp) * 500 / dt * 0.2f;
        force += dampingForce;
      }

      vec3 velocityNormalized = parentVelocity;// .logarithmic();

      frictionVelocity = parentVelocity.projectedOnVector(suspensionLeft).projectedOnPlane(normal);

      vec3 wheelSideVelocityFactor = parentVelocity.projectedOnVector(suspensionLeft).projectedOnPlane(normal);
      vec3 wheelForwardVelocityFactor = parentVelocity.projectedOnVector(suspensionForward).projectedOnPlane(normal);
      vec3 prevFrictionForce = frictionForce;

      frictionForce = vec3::zero;
      frictionForce = -wheelSideVelocityFactor * 50.0f / dt;
      frictionForce -= frictionForce.projectedOnVector(carForward) * mapRangeClamped(fabsf(wheelAngularVelocity), 0, 6, 0, 0.5f);
      frictionForce += -wheelForwardVelocityFactor * nForceScalar * wheelConfig.rollingFriction;
      frictionForce += (suspensionLeft % normal) * enginePower;
      frictionForce += -parentVelocity.projectedOnVector(suspensionLeft % normal) * brakePower;

      float maxFrictionForce = std::min(nForceScalar, 2000.0f) * wheelConfig.tireFriction * 10;

      if (frictionForce.sqLength() > sqr(maxFrictionForce))
        frictionForce = frictionForce.normalized() * maxFrictionForce;

      force += frictionForce; //prevFrictionForce * 0.8f + frictionForce * 0.2f;

      //vec3 engineForce = (suspensionLeft % normal).normalized() * enginePower;
      //force += engineForce;

      //vec3 brakeForce = -parentVelocity.logarithmic().projectedOnVector(suspensionLeft % normal) * brakePower;
      //force += brakeForce;

      if (debugName == "FrontLeftWheel")
      {
        int i = 0;
      }
    }

    suspensionOffset = clamp(suspensionOffset, -wheelConfig.maxSuspensionOffset, wheelConfig.maxSuspensionOffset);

    return force;
  }

  void Wheel::draw(bool drawWires)
  {
    if (isGrounded)
    {
      vec3 bottom = position + vec3{ 0, -wheelConfig.radius + 0.2f, 0 };
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