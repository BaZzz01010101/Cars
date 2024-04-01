#pragma once
#include "SemiVector.hpp"

namespace game
{

  struct CollisionGeometry
  {
    static constexpr int STATIC_COUNT = 5;
    SemiVector<Sphere, STATIC_COUNT> spheres;

    void add(vec3 position, float radius);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    bool collideWith(Sphere sphere, vec3* collisionPoint, vec3* collisionNormal, float* penetration) const;
    std::pair<vec3, vec3> getBounds() const;
  };

}