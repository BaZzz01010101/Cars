#pragma once
#include "Config.h"
#include "RigidBody.h"
#include "Renderable.h"

namespace game
{
  class TurretPlatform : public Renderable, public Object
  {
    public:
    void init(const Config::Physics::Turret& config, const Model& model, float scale);
    void update(float dt, const RigidBody& parent, vec3 parentConnectionPoint, vec3 target);

    void draw(bool drawWires);
    void drawDebug();
    void reset();
  };
}

