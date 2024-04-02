#pragma once

namespace game
{

  struct PlayerState
  {
    int index {};
    int uid {};
    vec3 position {};
    quat rotation {};
    vec3 velocity {};
    vec3 angularVelocity {};
    float gunYaw {};
    float gunPitch {};
    float cannonYaw {};
    float cannonPitch {};
  };

}
