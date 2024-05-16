#pragma once
#include <Terrain.h>
#include <Config.h>

namespace game
{

  struct CustomCamera
  {
    enum Mode
    {
      Normal,
      Zoom
    };

    const Config& config {};
    Camera camera {};

    float expectedYaw = 0;
    float expectedPitch = 0;
    float yaw = 0;
    float pitch = 0;
    vec3 focusPosition = vec3::zero;
    float distanceFromFocus = 0;
    vec3 position = vec3::zero;
    vec3 direction = vec3::forward;
    bool invertY = false;
    Mode mode = Mode::Normal;

    CustomCamera(const Config& config);
    CustomCamera(CustomCamera&) = delete;
    CustomCamera(CustomCamera&&) = delete;
    CustomCamera& operator=(CustomCamera&) = delete;
    CustomCamera& operator=(CustomCamera&&) = delete;

    void update(float dt, const Terrain& terrain, vec3 playerPosition);
    void reset(vec3 playerPosition);

    inline operator Camera() const { return camera; }
  };

}