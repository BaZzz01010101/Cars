#include "pch.h"
#include "TerrainObject.h"

namespace game
{
  TerrainObject::TerrainObject(const Model& model, vec3 position, float angle, float scale) :
    Renderable(model)
  {
    transform = MatrixMultiply(MatrixMultiply(QuaternionToMatrix(quat::fromYAngle(angle)), MatrixScale(scale, scale, scale)), MatrixTranslate(position.x, position.y, position.z));
  }
}