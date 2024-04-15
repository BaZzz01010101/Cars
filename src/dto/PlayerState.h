#pragma once

namespace dto
{
  using namespace game;

  struct PlayerState
  {
    uint64_t guid {};
    vec3 position {};
    quat rotation {};
    vec3 velocity {};
    vec3 angularVelocity {};
    float gunYaw {};
    float gunPitch {};
    float cannonYaw {};
    float cannonPitch {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerState&) const = default;
  };

}
