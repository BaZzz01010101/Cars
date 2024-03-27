#pragma once

namespace game
{

  struct sphere
  {
    vec3 center {};
    float radius {};

    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    bool collideWith(sphere other, vec3* collisionPoint, vec3* collisionNormal, float* penetration) const;
  };


}
