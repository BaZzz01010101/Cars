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

    vec3 lastPosition = vec3::zero;
    vec3 lastVelocity = vec3::zero;

    vec3 position = vec3::zero;
    vec3 velocity = vec3::zero;
    float gravity = 0;
    float lifeTime = 0;
    float size = 0;
    //TODO: Replace with ownerGuid to avoid incorrect damage calculation when index is reused by new connected player
    int ownerIndex = 0;
    int baseDamage = 0;
    Type type = Type::UNKNOWN;

    void update(float dt);
  };

}
