#include "pch.h"
#include "ExplosionParticle.h"
#include "Helpers.h"

namespace game{
  ExplosionParticle ExplosionParticle::random(vec3 position, float minSize, float maxSize, float minSpeed, float maxSpeed, float minAngularSpeed, float maxAngularSpeed, float minLifeTime, float maxLifeTime, float gravity)
  {
    vec2 v0 = vec2::randomInRing(minSize, maxSize);
    vec2 v1 = vec2::randomInRing(minSize, maxSize);
    vec2 v2 = -(v1 + v0).normalized() * randf(minSize, maxSize);

    return {
      .position = position,
      .velocity = vec3::randomInHollowSphere(minSpeed, maxSpeed),
      .rotation = quat::random(),
      .angularVelocity = vec3::randomInHollowSphere(minAngularSpeed, maxAngularSpeed),
      .lifeTime = randf(minLifeTime, maxLifeTime),
      .gravity = gravity,
      .vertices = {
        v0.intoXZWithY(0),
        v1.intoXZWithY(0),
        v2.intoXZWithY(0),
      }
    };
  }

  void ExplosionParticle::update(float dt)
  {
    position += velocity * dt;
    velocity -= 0.5f * velocity * dt;
    velocity.y -= gravity * dt;
    vec3 dr = angularVelocity * dt;
    rotation = rotation * quat::fromEuler(dr.y, dr.z, dr.x);
    rotation.normalize();
    lifeTime -= dt;
  }

  void ExplosionParticle::draw() const
  {
    vec3 v0 = position + vertices[0].rotatedBy(rotation);
    vec3 v1 = position + vertices[1].rotatedBy(rotation);
    vec3 v2 = position + vertices[2].rotatedBy(rotation);
    DrawTriangle3D(v0, v1, v2, WHITE);
    DrawTriangle3D(v0, v2, v1, WHITE);
  }

}
