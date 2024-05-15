#pragma once
#include "Config.h"
#include "DynamicObject.h"

namespace game
{

  struct ExplosionParticle : public DynamicObject
  {
    static ExplosionParticle random(vec3 position, float minSize, float maxSize, float minSpeed, float maxSpeed, float minAngularSpeed, float maxAngularSpeed, float minLifeTime, float maxLifeTime, float gravity);

    float lifeTime = 0;
    float gravity = 9.8f;
    vec3 vertices[3] = { vec3::zero,  vec3::zero,  vec3::zero };

    void update(float dt);
  };

}