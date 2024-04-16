#pragma once
#include "Object.h"

namespace game
{

  struct TerrainObject
  {
    enum Type
    {
      Tree1,
      Tree2,
      Rock,
    };

    Type type {};
    Matrix transform {};

    TerrainObject(Type type, vec3 position, float angle, float scale);
  };

}