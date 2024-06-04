#pragma once

namespace game
{

  struct Sphere
  {
    vec3 center {};
    float radius {};

    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    bool collideWith(Sphere other, vec3* collisionPoint, vec3* collisionNormal, float* penetration) const;
    void transformBy(vec3 position, quat rotation);
    Sphere transformedBy(vec3 position, quat rotation) const;
  };

}
