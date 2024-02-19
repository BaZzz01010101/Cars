#pragma once
#include "Object.h"

namespace game
{

  class DynamicObject : virtual public Object
  {
  public:
    vec3 velocity{};
    vec3 angularVelocity{};

    virtual ~DynamicObject() = default;

    virtual void update(float dt);
  };

}

