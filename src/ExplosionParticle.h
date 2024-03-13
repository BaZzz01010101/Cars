#pragma once
#include "Config.h"

namespace game
{

  struct ExplosionParticle
  {
    static ExplosionParticle random(vec3 position, float minSize, float maxSize, float minSpeed, float maxSpeed, float minAngularSpeed, float maxAngularSpeed, float minLifeTime, float maxLifeTime, float gravity);

    vec3 position = vec3::zero;
    vec3 velocity = vec3::zero;
    quat rotation = quat::identity;
    vec3 angularVelocity = vec3::zero;
    float lifeTime = 0;
    float gravity = 9.8f;
    vec3 vertices[3] = { vec3::zero,  vec3::zero,  vec3::zero };

    void update(float dt);
    void draw() const;
  };

}