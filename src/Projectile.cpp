#include "pch.h"
#include "Projectile.h"
#include "CustomCamera.h"

namespace game
{
  void Projectile::update(float dt)
  {
    position += velocity * dt;
    velocity -= 0.1f * velocity * dt;
    velocity.y -= gravity * dt;
    lifeTime -= dt;
  }

  void Projectile::draw()
  {
    DrawCapsule(position, position + 0.1f * size * velocity, size, 5, 2, WHITE);
  }

}
