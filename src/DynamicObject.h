#pragma once
#include "Object.h"

namespace game
{

  struct DynamicObject : public Object
  {
    vec3 velocity{};
    vec3 angularVelocity{};

    void update(float dt);
  };

}

