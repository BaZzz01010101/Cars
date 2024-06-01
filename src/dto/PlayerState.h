#pragma once
#include "WheelState.h"
#include "TurretState.h"

namespace dto
{
  using namespace game;

  struct PlayerState
  {
    int64_t physicalFrame {};
    uint64_t guid {};

    int ping {};
    vec3 position {};
    quat rotation {};
    vec3 velocity {};
    vec3 angularVelocity {};
    float steeringAngle {};
    int health {};
    float aliveStateTimeout {};

    WheelState frontLeftWheelState {};
    WheelState frontRightWheelState {};
    WheelState rearLeftWheelState {};
    WheelState rearRightWheelState {};

    TurretState gunState {};
    TurretState cannonState {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerState&) const = default;
  };

}
