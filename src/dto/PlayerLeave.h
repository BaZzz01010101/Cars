#pragma once

namespace dto
{
  using namespace game;

  struct PlayerLeave
  {
    uint64_t guid {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerLeave&) const = default;
  };

}
