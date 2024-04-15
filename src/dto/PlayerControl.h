#pragma once

namespace dto
{
  using namespace game;

  struct PlayerControl
  {
    uint64_t guid {};
    float steeringAxis {};
    float accelerationAxis {};
    float thrustAxis {};
    vec3 target {};
    bool primaryFire {};
    bool secondaryFire {};
    bool handBrake {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerControl&) const = default;
  };

}