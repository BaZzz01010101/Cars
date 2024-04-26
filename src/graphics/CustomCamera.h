#pragma once
#include <Terrain.h>
#include <Config.h>

namespace game
{

  struct CustomCamera
  {
    const Config& config {};
    Camera camera {};

    float yaw = 0;
    float pitch = 0;
    vec3 position = vec3::zero;
    vec3 direction = vec3::zero;
    bool invertY = false;

    CustomCamera(const Config& config);
    CustomCamera(CustomCamera&) = delete;
    CustomCamera(CustomCamera&&) = delete;
    CustomCamera& operator=(CustomCamera&) = delete;
    CustomCamera& operator=(CustomCamera&&) = delete;

    void update(float dt, const Terrain& terrain, vec3 playerPosition);
    void reset(vec3 playerPosition);

    vec3 getTarget() const;

    inline operator Camera() const { return camera; }
  };

}