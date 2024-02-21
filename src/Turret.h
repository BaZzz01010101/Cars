#pragma once
#include "Renderable.h"
#include "Config.h"
#include "Terrain.h"
#include "RigidBody.h"

namespace game
{
  class Turret : virtual public Renderable, virtual public Object
  {
  public:
    vec3 target{};

    void init(const Config::Physics::Turret& config, const Model& model, const Terrain& terrain, const Object& parent, vec3 parentConnectionPoint, float scale);
    void reset();

    virtual void update(float dt);

  protected:
    virtual void updateTransform();
    virtual void drawDebug();

  private:
    Config::Physics::Turret turretConfig{};
    const Terrain* terrain;
    const Object* parent;
    float scale{};
    vec3 parentConnectionPoint{};

    float yaw{};
    float pitch{};
  };
}
