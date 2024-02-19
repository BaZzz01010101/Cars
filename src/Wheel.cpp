#include "pch.h"
#include "Wheel.h"
#include "Helpers.h"

namespace game
{
  void Wheel::init(const Config::Physics::Wheels& config, const Model& model, const Terrain& terrain, const PhysicalObject& parent, vec3 parentConnectionPoint, const char* debugName, float gravity)
  {
    wheelConfig = config;
    this->terrain = &terrain;
    this->parent = &parent;
    this->parentConnectionPoint = parentConnectionPoint;
    this->debugName = debugName;
    this->gravity = gravity;
    this->momentOfInertia = 0.5f * wheelConfig.mass * sqr(wheelConfig.radius);
    Renderable::init(model);
  }
  
  void Wheel::update(float dt)
  {
    vec3 globalConnectionPoint = parentConnectionPoint.rotatedBy(parent->rotation);
    position = parent->position + globalConnectionPoint;
    rotation = parent->rotation * quat::fromYAngle(steeringAngle);
    velocity = parent->velocity + parent->angularVelocity.rotatedBy(rotation) % globalConnectionPoint;
    angularVelocity = parent->angularVelocity;

    float springForce = -suspensionOffset * wheelConfig.suspensionStiffness;
    suspensionSpeed += springForce / wheelConfig.mass * dt;
    float dampingForce = -suspensionSpeed * wheelConfig.mass / dt * wheelConfig.suspensionDamping;
    suspensionSpeed += dampingForce / wheelConfig.mass * dt;
    suspensionOffset += suspensionSpeed * dt;

    float terrainY = terrain->getHeight2(position.x, position.z, &normal);
    float bottomY = position.y + suspensionOffset - wheelConfig.radius;
    float penetration = terrainY - bottomY;
    isGrounded = penetration > 0;

    if (isGrounded)
    {
      suspensionOffset += penetration;
      suspensionSpeed = -velocity * vec3::up;
    }

    suspensionOffset = clamp(suspensionOffset, -wheelConfig.maxSuspensionOffset, wheelConfig.maxSuspensionOffset);
    wheelRotation.rotateByXAngle(wheelRotationSpeed * dt);
  }

  vec3 Wheel::getForce(float dt, float sharedMass, float enginePower, float brakePower, bool handBreaked)
  {
    vec3 force = vec3::zero;

    vec3 suspensionUp = vec3::up.rotatedBy(rotation);
    vec3 suspensionForward = vec3::forward.rotatedBy(rotation);
    vec3 suspensionLeft = vec3::left.rotatedBy(rotation);

    vec3 frictionUp = normal;
    vec3 frictionForward = suspensionForward.rotatedOnPlane(normal);
    vec3 frictionLeft = frictionUp % frictionForward;

    wheelRotationSpeed += enginePower / momentOfInertia * dt;
    wheelRotationSpeed = moveTo(wheelRotationSpeed, 0, std::max(wheelRotationSpeed * wheelConfig.rollingFriction, 0.01f));
    /*float maxRPS = float(!handBreaked) * (20 + 4 * velocity.length());
    wheelRotationSpeed = clamp(wheelRotationSpeed, -maxRPS, maxRPS);*/

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

      float frictionSpeed = (velocity.projectedOnPlane(normal) - wheelRotationSpeed * wheelConfig.radius * frictionForward).length();
      float frictionKoef = wheelConfig.tireFriction + mapRangeClamped(frictionSpeed, 5, 30, 0.0f, -0.1f);
      float maxFrictionForce = std::min(nForceScalar * frictionKoef, sharedMass * 100);

      vec3 lastFrictionForce = frictionForce;

      vec3 gravityVelocity = handBreaked ?
        vec3{ 0, -gravity * dt, 0 }.projectedOnPlane(normal) :
        vec3{ 0, -gravity * dt, 0 }.projectedOnVector(frictionLeft);

      vec3 frictionVelocityForecast = velocity + gravityVelocity - wheelRotationSpeed * wheelConfig.radius * frictionForward;
      frictionForce = -frictionVelocityForecast.projectedOnPlane(normal) * sharedMass / dt; 

      if (frictionForce.sqLength() > sqr(maxFrictionForce))
        frictionForce = frictionForce.normalized() * maxFrictionForce;

      frictionForce = 0.5 * frictionForce + 0.5 * lastFrictionForce;

      force += frictionForce;

      float targetWheelRotationSpeed = float(!handBreaked) * velocity * frictionForward / wheelConfig.radius;
      float step = (frictionForce * frictionForward) * wheelConfig.radius / momentOfInertia * dt;
      wheelRotationSpeed = moveTo(wheelRotationSpeed, targetWheelRotationSpeed, step);

      frictionVelocity = velocity.projectedOnPlane(normal) - wheelRotationSpeed * wheelConfig.radius * frictionForward;
      this->force = force;
    }

    return force;
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

  void Wheel::updateTransform()
  {
    transform = MatrixMultiply(QuaternionToMatrix(rotation * wheelRotation), MatrixTranslate(position.x, position.y + suspensionOffset, position.z));
  }

  void Wheel::drawDebug()
  {
    if (isGrounded)
    {
      vec3 bottom = position + vec3{ 0, -wheelConfig.radius + 0.2f, 0 };
      //DrawSphere(bottom, 0.3f, ORANGE);

      //drawVector(bottom, velocity, LIME);
      drawVector(bottom, frictionVelocity, GREEN);
      drawVector(position, 0.001f * force, RED);
      drawVector(bottom, 0.001f * frictionForce, ORANGE);
    }
  }

}