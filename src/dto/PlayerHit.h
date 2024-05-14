#pragma once

namespace dto
{
  using namespace game;

  struct PlayerHit
  {
    int64_t physicalFrame {};
    uint64_t guid {};

    uint64_t attakerGuid {};
    int damage {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerHit&) const = default;
  };

}