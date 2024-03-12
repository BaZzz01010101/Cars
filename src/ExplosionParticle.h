#pragma once
#include "Config.h"

namespace game
{

  struct ExplosionParticle
  {
    static ExplosionParticle random(vec3 position, float minSize, float maxSize, float minSpeed, float maxSpeed, float minAngularSpeed, float maxAngularSpeed, float minLifeTime, float maxLifeTime, float gravity);

    vec3 position{};
    vec3 velocity{};
    quat rotation{};
    vec3 angularVelocity{};
    float lifeTime{};
    float gravity{};
    vec3 vertices[3]{};

    void update(float dt);
    void draw() const;
  };

}