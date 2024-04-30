#include "core.h"
#include "Helpers.h"

namespace game
{
  // TODO: Move to helpers folder
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

  vec2 moveTo(vec2 current, vec2 target, float maxDelta)
  {
    maxDelta = fabsf(maxDelta);
    vec2 delta = target - current;
    float len = delta.length();

    if (len <= maxDelta)
      return target;
    else
      return current + delta / len * maxDelta;
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

  vec2 moveToRelative(vec2 current, vec2 target, float maxDeltaRelative)
  {
    maxDeltaRelative = fabsf(maxDeltaRelative);
    vec2 delta = target - current;
    float len = delta.length();
    float maxDelta = len * maxDeltaRelative;

    if (len <= maxDelta)
      return target;
    else
      return current + delta / len * maxDelta;
  }

  vec3 moveToRelative(vec3 current, vec3 target, float maxDeltaRelative)
  {
    maxDeltaRelative = fabsf(maxDeltaRelative);
    vec3 delta = target - current;
    float len = delta.length();
    float maxDelta = len * maxDeltaRelative;

    if (len <= maxDelta)
      return target;
    else
      return current + delta / len * maxDelta;
  }

  float moveToRelative(float current, float target, float maxDeltaRelative)
  {
    maxDeltaRelative = fabsf(maxDeltaRelative);
    float delta = target - current;
    float len = fabsf(delta);
    float maxDelta = len * maxDeltaRelative;

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
