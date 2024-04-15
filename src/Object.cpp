#include "core.h"
#include "Object.h"

namespace game
{
  // TODO: Move this and all descendants to objects folder
  vec3 game::Object::forward() const
  {
    return vec3::forward.rotatedBy(rotation);
  }

  vec3 game::Object::left() const
  {
    return vec3::left.rotatedBy(rotation);
  }

  vec3 game::Object::up() const
  {
    return vec3::up.rotatedBy(rotation);
  }

}