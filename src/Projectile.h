#pragma once
#include "DynamicObject.h"

namespace game
{
 
  struct Projectile
  {
    enum Type
    {
      UNKNOWN = 0,
      Bullet,
      Shell,
    };

    vec3 position{};
    vec3 velocity{};
    float gravity{};
    float lifeTime{};
    float size{};
    int ownerIndex{};
    float damage{};
    Type type{};

    void update(float dt);
    void draw() const;
  };

}
