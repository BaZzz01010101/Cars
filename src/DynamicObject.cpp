#include "core.h"
#include "DynamicObject.h"

namespace game
{
  void DynamicObject::update(float dt)
  {
    lastPosition = position;
    lastRotation = rotation;

    position += velocity * dt;
    vec3 dr = angularVelocity * dt;
    rotation = rotation * quat::fromEuler(dr.y, dr.z, dr.x);
    rotation.normalize();
  }
}