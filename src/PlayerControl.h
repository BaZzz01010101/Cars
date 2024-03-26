#pragma once

namespace game
{

  struct PlayerControl
  {
    int uid {};
    float steeringAxis {};
    float accelerationAxis {};
    float thrustAxis {};
    vec3 target {};
    bool primaryFire {};
    bool secondaryFire {};
    bool handBrake {};
  };

}