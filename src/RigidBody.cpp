#include "pch.h"
#include "RigidBody.h"
#include "Helpers.h"

namespace game
{
  void RigidBody::updateBody(float dt)
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

    position += velocity * dt;

    vec3 dr = angularVelocity * dt;
    rotation = rotation * quat::fromEuler(dr.y, dr.z, dr.x);
    rotation.normalize();
    rotation.toEuler(&eulerRotation.y, &eulerRotation.z, &eulerRotation.x);
  }

  void RigidBody::resetForces()
  {
    force = vec3::zero;
    moment = vec3::zero;
  }

  void RigidBody::applyLocalForceAtLocalPoint(vec3 force, vec3 point)
  {
    this->force += force.rotatedBy(rotation);
    this->moment += point % force;
  }

  void RigidBody::applyLocalForceAtCenterOfMass(vec3 force)
  {
    this->force += force.rotatedBy(rotation);
  }

  void RigidBody::applyGlobalForceAtGlobalPoint(vec3 force, vec3 point)
  {
    this->force += force;
    vec3 localForce = force.rotatedBy(rotation.inverted());
    vec3 localPoint = point.rotatedBy(rotation.inverted());
    this->moment += localPoint % localForce;
  }

  void RigidBody::applyGlobalForceAtLocalPoint(vec3 globalForce, vec3 localPoint)
  {
    this->force += globalForce;
    vec3 localForce = globalForce.rotatedBy(rotation.inverted());
    this->moment += localPoint % localForce;
  }

  void RigidBody::applyGlobalForceAtCenterOfMass(vec3 force)
  {
    this->force += force;
  }

  void RigidBody::applyMoment(vec3 moment)
  {
    this->moment += moment;
  }

  void RigidBody::applyGravity()
  {
    applyGlobalForceAtCenterOfMass(mass * vec3{ 0, -gravity, 0 });
  }

  vec3 RigidBody::forward() const
  {
    return vec3::forward.rotatedBy(rotation);
  }

  vec3 RigidBody::left() const
  {
    return vec3::left.rotatedBy(rotation);
  }

  vec3 RigidBody::up() const
  {
    return vec3::up.rotatedBy(rotation);
  }
}