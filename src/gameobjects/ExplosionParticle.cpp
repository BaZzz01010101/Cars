#include "core.h"
#include "ExplosionParticle.h"
#include "Helpers.h"

namespace game
{
  ExplosionParticle ExplosionParticle::random(vec3 position, float minSize, float maxSize, float minSpeed, float maxSpeed, float minAngularSpeed, float maxAngularSpeed, float minLifeTime, float maxLifeTime, float gravity)
  {
    vec2 v0 = vec2::randomInRing(minSize, maxSize);
    vec2 v1 = vec2::randomInRing(minSize, maxSize);
    vec2 v2 = -(v1 + v0).normalized() * randf(minSize, maxSize);
    quat rotation = quat::random();

    ExplosionParticle p;
    p.position = position;
    p.rotation = rotation;
    p.lastPosition = position;
    p.lastRotation = rotation;
    p.velocity = vec3::randomInHollowSphere(minSpeed, maxSpeed);
    p.angularVelocity = vec3::randomInHollowSphere(minAngularSpeed, maxAngularSpeed);
    p.lifeTime = randf(minLifeTime, maxLifeTime);
    p.gravity = gravity;
    p.vertices[0] = v0.intoXZWithY(0);
    p.vertices[1] = v1.intoXZWithY(0);
    p.vertices[2] = v2.intoXZWithY(0);

    return p;
  }

  void ExplosionParticle::update(float dt)
  {
    DynamicObject::update(dt);

    velocity -= 0.5f * velocity * dt;
    velocity.y -= gravity * dt;
    lifeTime -= dt;
  }

}
