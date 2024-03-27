#include "pch.h"
#include "Helpers.h"

namespace game
{
  int gSeed = 0;

  float mapRangeClamped(float value, float min1, float max1, float min2, float max2)
  {
    if (min1 == max1)
      return (min2 + max2) / 2;

    float delta1 = max1 - min1;
    float delta2 = max2 - min2;
    value = clamp(value, min1, max1);

    return min2 + (value - min1) * delta2 / delta1;
  }

  vec3 moveTo(vec3 current, vec3 target, float maxDelta)
  {
    maxDelta = fabsf(maxDelta);
    vec3 delta = target - current;
    float len = delta.length();

    if (len <= maxDelta)
      return target;
    else
      return current + delta / len * maxDelta;
  }

  float moveTo(float current, float target, float maxDelta)
  {
    maxDelta = fabsf(maxDelta);
    float delta = target - current;
    float len = fabsf(delta);

    if (len <= maxDelta)
      return target;
    else
      return current + delta / len * maxDelta;
  }

  float moveAngleTo(float current, float target, float maxDelta)
  {
    float delta = normalizeAngle(target - current);
    float len = fabsf(delta);

    if (len <= maxDelta)
      return target;
    else
      return normalizeAngle(current + sign(delta) * maxDelta);
  }

  int fastRand(void)
  {
    gSeed = 1664525u * gSeed + 1013904223u;

    return (gSeed ^ (gSeed >> 16u)) & FAST_RAND_MAX;
  }

}
