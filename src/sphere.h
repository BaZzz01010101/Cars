#pragma once

namespace game
{

  struct sphere
  {
    vec3 position {};
    float radius {};

    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
  };


}
