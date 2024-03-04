#pragma once
#include "Renderable.h"
#include "Config.h"

namespace game
{

  class Explosion : public Renderable
  {
  public:
    Explosion() = default;
    Explosion(Explosion&) = delete;
    virtual ~Explosion();
    Explosion& operator=(Explosion&) = delete;

    void init(const Config& config, const vec3& position, float radius, float duration);
    void update(float dt);
    void draw() const;
  };

}

