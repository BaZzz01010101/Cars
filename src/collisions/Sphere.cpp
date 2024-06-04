#include "core.h"
#include "Sphere.h"
#include "Helpers.h"

namespace game
{

  bool Sphere::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    assert(directionNormalized.isNormalized());

    vec3 originToCenter = center - origin;
    float originToCenterSqLength = originToCenter.sqLength();
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

  bool Sphere::collideWith(Sphere other, vec3* collisionPoint, vec3* collisionNormal, float* penetration) const
  {
    vec3 centerToCenter = other.center - center;
    float centerToCenterSqLength = centerToCenter.sqLength();
    float radiusSum = other.radius + radius;
    float sqRadiusSum = sqr(radiusSum);

    if (centerToCenterSqLength > sqRadiusSum)
      return false;

    if (centerToCenterSqLength < EPSILON)
    {
      if (collisionPoint)
        *collisionPoint = center + vec3 { 0, radiusSum, 0 };

      if (collisionNormal)
        *collisionNormal = vec3::up;

      if (penetration)
        *penetration = radiusSum;

      return true;
    }

    float centerToCenterLength = sqrt(centerToCenterSqLength);
    vec3 centerToCenterNormalized = centerToCenter / centerToCenterLength;

    if (collisionPoint)
      *collisionPoint = center + centerToCenterNormalized * radius;

    if (collisionNormal)
      *collisionNormal = centerToCenterNormalized;

    if (penetration)
      *penetration = radiusSum - centerToCenterLength;

    return true;
  }

  void Sphere::transformBy(vec3 position, quat rotation)
  {
    center.rotateBy(rotation);
    center += position;
  }

  Sphere Sphere::transformedBy(vec3 position, quat rotation) const
  {
    return {
      .center = center.rotatedBy(rotation) + position,
      .radius = radius,
    };
  }

}