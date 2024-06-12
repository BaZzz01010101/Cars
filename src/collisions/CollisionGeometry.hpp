#pragma once
#include "HybridVector.hpp"

namespace game
{

  template <int StaticSize>
  struct CollisionGeometry
  {
    HybridVector<Sphere, StaticSize> spheres;

    void add(vec3 position, float radius);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    bool collideWith(Sphere sphere, vec3* collisionPoint, vec3* collisionNormal, float* penetration) const;
    std::pair<vec3, vec3> getBounds() const;
  };

  template <int StaticSize>
  void CollisionGeometry<StaticSize>::add(vec3 position, float radius)
  {
    spheres.add({ position, radius });
  }

  template <int StaticSize>
  bool CollisionGeometry<StaticSize>::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    float closestDistance = FLT_MAX;
    vec3 closestHitPosition {};
    vec3 closestNormal {};

    float currentDistance = 0;
    vec3 currentHitPosition, currentNormal;

    for (int i = 0; i < spheres.size(); i++)
      if (spheres[i].traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentNormal, &currentDistance))
        if (currentDistance < closestDistance)
        {
          closestDistance = currentDistance;
          closestHitPosition = currentHitPosition;
          closestNormal = currentNormal;
        }

    if (closestDistance == FLT_MAX)
      return false;

    if (hitPosition)
      *hitPosition = closestHitPosition;

    if (hitNormal)
      *hitNormal = closestNormal;

    if (hitDistance)
      *hitDistance = closestDistance;

    return true;
  }

  template <int StaticSize>
  bool CollisionGeometry<StaticSize>::collideWith(Sphere sphere, vec3* collisionPoint, vec3* collisionNormal, float* penetration) const
  {
    float avgPenetration = 0;
    vec3 avgCollisionPosition {};
    vec3 avgCollisionNormal {};
    int collisionCount = 0;

    for (int i = 0; i < spheres.size(); i++)
      if (spheres[i].collideWith(sphere, collisionPoint, collisionNormal, penetration))
      {
        if (collisionPoint)
          avgCollisionPosition += *collisionPoint;

        if (collisionNormal)
          avgCollisionNormal += *collisionNormal;

        if (penetration)
          avgPenetration += *penetration;

        collisionCount++;
      }

    if (collisionCount == 0)
      return false;

    if (collisionPoint)
      *collisionPoint = avgCollisionPosition / float(collisionCount);

    if (collisionNormal)
      *collisionNormal = avgCollisionNormal / float(collisionCount);

    if (penetration)
      *penetration = avgPenetration / float(collisionCount);

    return true;
  }

  template <int StaticSize>
  std::pair<vec3, vec3> CollisionGeometry<StaticSize>::getBounds() const
  {
    vec3 min = vec3::max;
    vec3 max = vec3::min;

    for (int i = 0; i < spheres.size(); i++)
    {
      const Sphere& sphere = spheres[i];
      vec3 position = sphere.center;
      float radius = sphere.radius;

      min.x = std::min(min.x, position.x - radius);
      min.y = std::min(min.y, position.y - radius);
      min.z = std::min(min.z, position.z - radius);

      max.x = std::max(max.x, position.x + radius);
      max.y = std::max(max.y, position.y + radius);
      max.z = std::max(max.z, position.z + radius);
    }

    return { min, max };
  }

}