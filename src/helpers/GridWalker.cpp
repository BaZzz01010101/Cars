#include "core.h"
#include "Helpers.h"
#include "GridWalker.h"

namespace game
{
  // TODO: Move to helpers folder
  GridWalker::GridWalker(const box2&& spaceBounds, float cellSize) :
    spaceBounds(spaceBounds),
    spaceOrigin(-spaceBounds.min),
    cellSize(cellSize)
  {
    _ASSERT(spaceBounds.width() > 0);
    _ASSERT(spaceBounds.height() > 0);
    _ASSERT(cellSize > 0);

    gridWidth = (int)std::ceil(spaceBounds.width() / cellSize);
    gridHeight = (int)std::ceil(spaceBounds.height() / cellSize);
  }

  bool GridWalker::walkByLine(vec3 origin, vec3 directionNormalized, float distance, Callback callback)
  {
    assert(directionNormalized.isNormalized());

    distance = (distance < 0) ? FLT_MAX : distance;

    if (directionNormalized.isZero() || distance < EPSILON)
      return false;

    float maxAxis = std::max(fabsf(directionNormalized.x), fabsf(directionNormalized.z));

    if (maxAxis < EPSILON)
    {
      int x = (int)((origin.x + spaceOrigin.x) / cellSize);
      int y = (int)((origin.z + spaceOrigin.y) / cellSize);

      if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight)
        return callback(x, y);

      return false;
    }

    vec2 direction2D = directionNormalized.xz();
    vec2 step = direction2D / maxAxis * 0.99f;
    float stepLength = step.length();
    vec2 origin2D = origin.xz();
    float distance2D = std::max(distance * direction2D.length() / cellSize, EPSILON);

    vec2 current = (origin2D + spaceOrigin) / cellSize;
    distance2D += float(M_SQRT2);

    int x = (int)current.x;
    int y = (int)current.y;

    while (distance2D > 0)
    {
      if (x < 0 && step.x <= 0 || x >= gridWidth && step.x >= 0 ||
          y < 0 && step.y <= 0 || y >= gridHeight && step.y >= 0)
        return false;

      if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight && callback(x, y))
        return true;

      vec2 next = current + step;
      int nx = (int)next.x;
      int ny = (int)next.y;

      // diagonal step
      if (nx != x && ny != y)
      {
        vec2 gridPoint = vec2 { float(std::min(x, nx) + 1), float(std::min(y, ny) + 1) };
        vec2 toGridPoint = gridPoint - current;
        vec2 right = vec2 { toGridPoint.y, -toGridPoint.x };
        float k = direction2D * right * sign(step.x) * sign(step.y);

        if (k > 0)
        {
          if (nx >= 0 && nx < gridWidth && y >= 0 && y < gridHeight && callback(nx, y))
            return true;
        }
        else if (k < 0)
        {
          if (x >= 0 && x < gridWidth && ny >= 0 && ny < gridHeight && callback(x, ny))
            return true;
        }
      }

      x = nx;
      y = ny;
      current = next;
      distance2D -= stepLength;
    }

    return false;
  }
}
