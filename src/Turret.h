#pragma once
#include "Renderable.h"
#include "Object.h"
#include "Config.h"
#include "Terrain.h"

namespace game
{
  class Turret : public Renderable, public Object
  {
  public:
    vec3 expectedTarget{};
    vec3 currentTarget{};

    Turret(const Config::Physics::Turret& config, const Model& model, const Terrain& terrain, vec3 parentConnectionPoint, float scale);
    Turret(Turret&) = delete;
    Turret(Turret&&) = delete;
    Turret& operator=(Turret&) = delete;
    Turret& operator=(Turret&&) = delete;

    void reset();
    vec3 barrelPosition() const;

    void update(float dt, const Object& parent);
    void draw(bool drawWires);

  private:
    const Config::Physics::Turret& config{};
    const Terrain& terrain;
    float scale{};
    vec3 connectionPoint{};

    float yaw{};
    float pitch{};

    void drawDebug();
  };
}
