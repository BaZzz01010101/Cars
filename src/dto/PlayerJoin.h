#pragma once

namespace dto
{
  using namespace game;

  struct PlayerJoin
  {
    int64_t physicalFrame {};
    uint64_t guid {};

    PlayerName name = DEFAULT_PLAYER_NAME;
    vec3 position = vec3::zero;
    quat rotation = quat::identity;

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerJoin&) const = default;
  };

}
