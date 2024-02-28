#pragma once
#include "Object.h"

namespace game
{

  class CollidableObject
  {
  public:
    virtual ~CollidableObject() = default;

    virtual bool traceRay(vec3 origin, vec3 direction, vec3* collision, vec3* normal) = 0;
    virtual bool collideWith(const CollidableObject& other, vec3* collision, vec3* normal, float* penetration) = 0;
  };

}