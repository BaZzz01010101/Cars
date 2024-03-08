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

    CustomCamera(const Config& config);
    CustomCamera(CustomCamera&) = delete;
    CustomCamera(CustomCamera&&) = delete;
    CustomCamera& operator=(CustomCamera&) = delete;
    CustomCamera& operator=(CustomCamera&&) = delete;

    void update(float dt, const Terrain& terrain, vec3 playerPosition);
    void reset(vec3 playerPosition);

    operator Camera() const { return camera; }

  private:
    const Config& config{};

    float yaw{};
    float pitch{};
  };

}