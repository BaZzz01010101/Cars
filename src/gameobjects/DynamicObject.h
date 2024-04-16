#pragma once
#include "Object.h"

namespace game
{

  struct DynamicObject : public Object
  {
    vec3 lastPosition = vec3::zero;
    quat lastRotation = quat::identity;

    vec3 velocity = vec3::zero;
    vec3 angularVelocity = vec3::zero;

    void update(float dt);
  };

}

