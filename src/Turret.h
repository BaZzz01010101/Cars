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
    vec3 target{};
    vec3 rayHitPosition{};
    bool isRayHit{};

    void init(const Config::Physics::Turret& config, const Model& model, const Terrain& terrain, const Object& parent, vec3 parentConnectionPoint, float scale);
    void reset();
    vec3 barrelPosition() const;

    void update(float dt);
    void draw(bool drawWires);

  private:
    Config::Physics::Turret turretConfig{};
    const Terrain* terrain;
    const Object* parent;
    float scale{};
    vec3 parentConnectionPoint{};

    float yaw{};
    float pitch{};

    void drawDebug();
  };
}
