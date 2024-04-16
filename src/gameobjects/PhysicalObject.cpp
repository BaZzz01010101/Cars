#include "core.h"
#include "PhysicalObject.h"

namespace game
{
  void PhysicalObject::update(float dt)
  {
    velocity += force / mass * dt;
    angularVelocity += moment / momentOfInertia * dt;

    if (velocity.sqLength() > 10000)
      velocity = velocity.normalized() * 100;

    if (angularVelocity.sqLength() > 100)
      angularVelocity = angularVelocity.normalized() * 10;

    const float DELTA = 0.01f;
    velocity.zeroIfLessThen(DELTA);
    angularVelocity.zeroIfLessThen(DELTA);
    force.zeroIfLessThen(DELTA);
    moment.zeroIfLessThen(DELTA);

    DynamicObject::update(dt);
  }

  void PhysicalObject::resetForces()
  {
    force = vec3::zero;
    moment = vec3::zero;
  }

  void PhysicalObject::applyGlobalForceAtLocalPoint(vec3 globalForce, vec3 localPoint)
  {
    this->force += globalForce;
    vec3 localForce = globalForce.rotatedBy(rotation.inverted());
    this->moment += localPoint % localForce;
  }

  void PhysicalObject::applyGlobalForceAtGlobalPoint(vec3 force, vec3 point)
  {
    this->force += force;
    vec3 localForce = force.rotatedBy(rotation.inverted());
    vec3 localPoint = point.rotatedBy(rotation.inverted());
    this->moment += localPoint % localForce;
  }

  void PhysicalObject::applyGlobalForceAtCenterOfMass(vec3 force)
  {
    this->force += force;
  }

  void PhysicalObject::applyMoment(const vec3 moment)
  {
    this->moment += moment;
  }
}