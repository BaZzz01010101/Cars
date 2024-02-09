#pragma once
#include "Positionable.h"

namespace game
{

  class Movable : public Positionable
  {
  public:
    vec3 velocity{};
    vec3 angularVelocity{};

    void update(float dt);
  };

}

