#pragma once
#include "DynamicObject.h"

namespace game
{
 
  class Projectile
  {
  public:
    vec3 position{};
    vec3 velocity{};
    float gravity{};
    float lifeTime{};
    float size{};
    int ownerIndex{};
    float damage{};

    void update(float dt);
    void draw();
  };

}
