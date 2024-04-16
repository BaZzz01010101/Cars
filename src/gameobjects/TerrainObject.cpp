#include "core.h"
#include "TerrainObject.h"

namespace game
{
  TerrainObject::TerrainObject(Type type, vec3 position, float angle, float scale) :
    type(type)
  {
    transform = MatrixMultiply(MatrixMultiply(QuaternionToMatrix(quat::fromYAngle(angle)), MatrixScale(scale, scale, scale)), MatrixTranslate(position.x, position.y, position.z));
  }
}