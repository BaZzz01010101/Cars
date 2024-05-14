#pragma once

namespace dto
{
  using namespace game;

  struct PlayerKill
  {
    int64_t physicalFrame {};
    uint64_t guid {};

    uint64_t killerGuid {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerKill&) const = default;
  };

}