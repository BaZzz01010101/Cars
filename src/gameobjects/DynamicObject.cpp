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

    // TODO: consider replace with more performant solution
    /*
      quat omega = quat { dr.x, dr.y, dr.z, 0.0f };
      quat q = rotation * omega;
      rotation.x += 0.5f * q.x;
      rotation.y += 0.5f * q.y;
      rotation.z += 0.5f * q.z;
      rotation.w += 0.5f * q.w;
  
      rotation.normalize();
    */
  }
}