#include "pch.h"
#include "CollisionGeometry.h"
#include "Helpers.h"

namespace game
{
  void CollisionGeometry::add(vec3 position, float radius)
  {
    spheres.add({ position, radius });
  }

  bool CollisionGeometry::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
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

  std::pair<vec3, vec3> CollisionGeometry::getBounds() const
  {
    vec3 min = vec3::max;
    vec3 max = vec3::min;

    for (int i = 0; i < spheres.size(); i++)
    {
      const Sphere& sphere = spheres[i];
      vec3 position = sphere.position;
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

  void CollisionGeometry::drawDebug() const
  {
    for (int i = 0; i < spheres.size(); i++)
      DrawSphereWires(spheres[i].position, spheres[i].radius, 8, 8, YELLOW);
  }

  bool CollisionGeometry::Sphere::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    vec3 originToCenter = position - origin;
    float originToCenterSqLength = originToCenter.sqLength();
    float sqDistance = sqr(distance);
    float sqRadius = sqr(radius);

    if (originToCenterSqLength > sqr(radius + distance))
      return false;

    float originToClosestRayPointLength = originToCenter * directionNormalized;

    if (originToClosestRayPointLength < 0)
      return false;

    float originToClosestRayPointSqLength = sqr(originToClosestRayPointLength);
    float centerToClosestRayPointSqLength = originToCenterSqLength - originToClosestRayPointSqLength;

    if (centerToClosestRayPointSqLength > sqRadius)
      return false;

    float originToHitPointLength = originToClosestRayPointLength - sqrt(sqRadius - centerToClosestRayPointSqLength);

    if (originToHitPointLength > distance)
      return false;

    if (hitPosition)
      *hitPosition = origin + directionNormalized * originToHitPointLength;

    if (hitNormal)
      *hitNormal = (directionNormalized * originToHitPointLength - originToCenter).normalized();

    if (hitDistance)
      *hitDistance = originToHitPointLength;

    return true;
  }
}
