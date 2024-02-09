#include "pch.h"
#include "Movable.h"

namespace game
{
  void Movable::update(float dt)
  {
    position += velocity * dt;
    vec3 dr = angularVelocity * dt;
    rotation = rotation * quat::fromEuler(dr.y, dr.z, dr.x);
    rotation.normalize();
  }
}