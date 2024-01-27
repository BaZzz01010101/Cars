#include "pch.h"
#include "RigidBody.h"
#include "Helpers.h"

namespace game
{
  void RigidBody::init()
  {
  }

  void RigidBody::updateBody(float dt)
  {
    velocity += force / mass * dt;
    angularVelocity += moment / momentOfInertia * dt;

    //const float DELTA = 0.001f;
    //velocity.zeroIfLessThen(DELTA);
    //angularVelocity.zeroIfLessThen(DELTA);
    //force.zeroIfLessThen(DELTA);
    //moment.zeroIfLessThen(DELTA);

    position += velocity * dt;

    vec3 dr = angularVelocity * dt;
    rotation = rotation * quat::fromEuler(dr.y, dr.z, dr.x);
    rotation.normalize();
  }

  void RigidBody::resetForces()
  {
    force = vec3::zero;
    moment = vec3::zero;
  }

  void RigidBody::applyForceLocal(vec3 force, vec3 point)
  {
    point.y += 0.27;
    this->force += force.rotatedBy(rotation);
    this->moment += point % force;
  }

  void RigidBody::applyForceLocal(vec3 force)
  {
    this->force += force.rotatedBy(rotation);
  }

  void RigidBody::applyForceGlobal(vec3 force, vec3 point)
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

  void RigidBody::applyForceGlobal(vec3 force)
  {
    this->force += force;
  }

  void RigidBody::applyMoment(vec3 moment)
  {
    this->moment += moment;
  }

  void RigidBody::applyImpulseLocal(vec3 impulse, vec3 point)
  {
    velocity += impulse / mass;
    angularVelocity += (point % impulse) / momentOfInertia;
  }

  void RigidBody::applyImpulseGlobal(vec3 impulse, vec3 point)
  {
    velocity += impulse / mass;
    vec3 localImpulse = impulse.rotatedBy(rotation.inverted());
    vec3 localPoint = point.rotatedBy(rotation.inverted());
    angularVelocity += (localPoint % localImpulse) / momentOfInertia;
  }

  void RigidBody::applyGravity()
  {
    applyForceGlobal(mass * vec3{ 0, GRAVITY , 0 });
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