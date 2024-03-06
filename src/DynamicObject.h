#pragma once
#include "Object.h"

namespace game
{

  class DynamicObject : public Object
  {
  public:
    vec3 velocity{};
    vec3 angularVelocity{};

    void update(float dt);
  };

}

