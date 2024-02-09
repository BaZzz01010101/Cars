#include "pch.h"
#include "Positionable.h"

namespace game
{

  vec3 game::Positionable::forward() const
  {
    return vec3::forward.rotatedBy(rotation);
  }

  vec3 game::Positionable::left() const
  {
    return vec3::left.rotatedBy(rotation);
  }

  vec3 game::Positionable::up() const
  {
    return vec3::up.rotatedBy(rotation);
  }

}