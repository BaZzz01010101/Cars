#include "core.h"
#include "Projectile.h"
#include "CustomCamera.h"

namespace game
{

  void Projectile::update(float dt)
  {
    lastPosition = position;
    lastVelocity = velocity;

    position += velocity * dt;
    velocity -= 0.1f * velocity * dt;
    velocity.y -= gravity * dt;
    lifeTime -= dt;
  }

}
