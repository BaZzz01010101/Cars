#pragma once
#include "Renderable.h"
#include "Config.h"
#include "Terrain.h"
#include "RigidBody.h"

namespace game
{
  class Turret : public Renderable, public Positionable
  {
  public:
    void init(const Config::Physics::Turret& config, const Model& model, float scale);
    void update(float dt, const Terrain& terrain, const Positionable& parent, vec3 parentConnectionPoint, vec3 target);

    void draw(bool drawWires);
    void drawDebug();
    void reset();

    private:
      Config::Physics::Turret turretConfig{};
      float scale{};

      vec3 position{};
      quat rotation{};
      float yaw{};
      float pitch{};
  };
}
