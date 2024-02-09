#pragma once
#include <Terrain.h>
#include <Config.h>

namespace game
{

  class CustomCamera
  {
  public:
    Camera camera{};
    vec3 position{};
    vec3 direction{};

    void init(const Config::Graphics& config);
    void update(float dt, const Terrain& terrain, vec3 playerPosition);
    void reset(vec3 playerPosition);

    operator Camera() const { return camera; }

  private:
    Config::Graphics graphicsConfig{};

    float yaw{};
    float pitch{};
  };

}