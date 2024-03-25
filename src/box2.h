#pragma once

namespace game
{

  struct box2
  {
    box2() = default;

    box2(float minX, float minY, float maxX, float maxY);
    box2(vec2 min, vec2 max);

    vec2 min {};
    vec2 max {};

    float width() const { return max.x - min.x; }
    float height() const { return max.y - min.y; }
  };

}