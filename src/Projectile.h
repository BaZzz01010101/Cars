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

    vec3 position = vec3::zero;
    vec3 velocity = vec3::zero;
    float gravity = 0;
    float lifeTime = 0;
    float size = 0;
    int ownerIndex = 0;
    float damage = 0;
    Type type = Type::UNKNOWN;

    void update(float dt);
    void draw() const;
  };

}
