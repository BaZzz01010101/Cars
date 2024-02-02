#include "pch.h"
#include "Wheel.h"
#include "Helpers.h"

namespace game
{
  void Wheel::init(const Config::Physics::Wheels& config, const Model& model, const char* debugName, float gravity)
  {
    this->debugName = debugName;
    this->gravity = gravity;
    wheelConfig = config;
    Renderable::init(model);
  }

  void Wheel::update(float dt, const Terrain& terrain, const RigidBody& parent, vec3 parentConnectionPoint, float steeringAngle)
  {
    vec3 globalConnectionPoint = parentConnectionPoint.rotatedBy(parent.rotation);
    position = parent.position + globalConnectionPoint;
    rotation = parent.rotation * quat::fromEuler(steeringAngle, 0, 0);
    velocity = parent.velocity + parent.angularVelocity.rotatedBy(rotation) % globalConnectionPoint;
    angularVelocity = parent.angularVelocity;

    float springForce = -suspensionOffset * wheelConfig.suspensionStiffness;
    suspensionSpeed += springForce / wheelConfig.mass * dt;
    float dampingForce = -suspensionSpeed * wheelConfig.mass / dt * wheelConfig.suspensionDamping;
    suspensionSpeed += dampingForce / wheelConfig.mass * dt;
    suspensionOffset += suspensionSpeed * dt;

    float terrainY = terrain.getHeight2(position.x, position.z, &normal);
    float bottomY = position.y + suspensionOffset - wheelConfig.radius;
    float MAX_PENETRATION = 0.1f;
    float penetration = std::max(terrainY - bottomY, 0.0f) / MAX_PENETRATION;

    if (penetration > 0)
    {
      suspensionOffset += terrainY - bottomY;
      suspensionSpeed = -velocity * vec3::up;
    }

    suspensionOffset = clamp(suspensionOffset, -wheelConfig.maxSuspensionOffset, wheelConfig.maxSuspensionOffset);

    wheelRotation = quat::fromEuler(0, 0, wheelRotationSpeed * dt) * wheelRotation;
    isGrounded = penetration > 0;
  }

  vec3 Wheel::getForce(float dt, float sharedMass, float aerodinamicForce, float enginePower, float brakePower, bool handBreaked)
  {
    vec3 force = vec3::zero;

    vec3 suspensionUp = vec3::up.rotatedBy(rotation);
    vec3 suspensionForward = vec3::forward.rotatedBy(rotation);
    vec3 suspensionLeft = vec3::left.rotatedBy(rotation);

    vec3 frictionUp = normal;
    vec3 frictionForward = suspensionForward.projectedOnPlane(normal);
    vec3 frictionLeft = frictionUp % frictionForward;

    float frictionSpeed = (velocity.projectedOnPlane(normal) - wheelRotationSpeed * wheelConfig.radius * frictionForward).length();

    float momentOfInertia = 0.5f * wheelConfig.mass * sqr(wheelConfig.radius);
    wheelRotationSpeed += enginePower / momentOfInertia * dt;
    wheelRotationSpeed = moveTo(wheelRotationSpeed, 0, std::max(wheelRotationSpeed * wheelConfig.rollingFriction, 0.1f));
    float maxRPS = float(!handBreaked) * (20 + 2 * velocity.length());
    wheelRotationSpeed = clamp(wheelRotationSpeed,  -maxRPS, maxRPS);

    if (isGrounded)
    {
      float nForceScalar = sqr(suspensionOffset) * wheelConfig.suspensionStiffness;
      nForce = nForceScalar * normal;
      force += nForce;

      float parentNormalSpeed = velocity * normal;

      if (parentNormalSpeed < 0)
      {
        vec3 dampingForce = -parentNormalSpeed * sharedMass / dt * wheelConfig.suspensionDamping * normal;
        force += dampingForce;
      }

      float frictionKoef = wheelConfig.tireFriction + mapRangeClamped(frictionSpeed, 10, 20, 0.0f, -0.2f);
      float maxFrictionForce = std::min(nForce * vec3::up + aerodinamicForce, 50000.0f) * 1 * frictionKoef;

      vec3 velocityForecast = velocity + vec3{ 0, -gravity * dt, 0 };

      frictionForce = -(velocityForecast.projectedOnPlane(normal) * sharedMass / dt).projectedOnVector(frictionLeft);

      float tireFrictionSpeed = (wheelRotationSpeed * wheelConfig.radius - velocity * frictionForward);
      //frictionForce += (tireFrictionSpeed * frictionForward * sharedMass / dt) * 0.7;
      frictionForce += tireFrictionSpeed * frictionForward / (sqr(1.69) / 270 + 1 / sharedMass) / dt;

      if (frictionForce.sqLength() > sqr(maxFrictionForce))
        frictionForce = frictionForce.normalized() * maxFrictionForce;

      float targetWheelRotationSpeed = float(!handBreaked) * velocity * frictionForward / wheelConfig.radius;
      float step = (frictionForce * frictionForward) * wheelConfig.radius / momentOfInertia * dt;
      wheelRotationSpeed = moveTo(wheelRotationSpeed, targetWheelRotationSpeed, step);

      force += frictionForce;

      frictionVelocity = velocity.projectedOnPlane(normal) - wheelRotationSpeed * wheelConfig.radius * frictionForward;
    }

    return force;
  }

  void Wheel::draw(bool drawWires)
  {
    Matrix transform = MatrixMultiply(QuaternionToMatrix(rotation * wheelRotation), MatrixTranslate(position.x, position.y + suspensionOffset, position.z));
    Renderable::draw(transform, drawWires);

    drawDebug();
  }

  void Wheel::drawDebug()
  {
    if (isGrounded)
    {
      vec3 bottom = position + vec3{ 0, -wheelConfig.radius + 0.2f, 0 };
      //DrawSphere(bottom, 0.3f, ORANGE);
      //drawVector(bottom, velocity, LIME);
      drawVector(bottom, frictionVelocity, GREEN);
      drawVector(bottom, 0.001f * frictionForce, ORANGE);
    }
  }

  void Wheel::reset()
  {
    position = vec3::zero;
    rotation = quat::identity;
    velocity = vec3::zero;
    angularVelocity = vec3::zero;
    wheelRotation = quat::identity;
    wheelRotationSpeed = 0;
    suspensionOffset = 0;
    suspensionSpeed = 0;
    nForce = vec3::zero;
    frictionForce = vec3::zero;
    frictionVelocity = vec3::zero;
  }
}