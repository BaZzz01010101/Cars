#include "pch.h"
#include "sphere.h"
#include "Helpers.h"

namespace game
{

  bool sphere::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    vec3 originToCenter = position - origin;
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

}