#include "pch.h"
#include "DynamicObject.h"

namespace game
{
  void DynamicObject::update(float dt)
  {
    position += velocity * dt;
    vec3 dr = angularVelocity * dt;
    rotation = rotation * quat::fromEuler(dr.y, dr.z, dr.x);
    rotation.normalize();
  }
}