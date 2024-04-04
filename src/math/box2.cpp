#include "core.h"
#include "box2.h"

namespace game
{
  box2::box2(float minX, float minY, float maxX, float maxY) :
    min { minX, minY }, max { maxX, maxY }
  {
  }

  box2::box2(vec2 min, vec2 max) :
    min(min), max(max)
  {
  }

}