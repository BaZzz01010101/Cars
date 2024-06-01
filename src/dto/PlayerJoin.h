#pragma once

namespace dto
{
  using namespace game;

  struct PlayerJoin
  {
    int64_t physicalFrame {};
    uint64_t guid {};

    PlayerName name = DEFAULT_PLAYER_NAME;
    // TODO: find a way to not send position and rotation for a players who currently respawning
    vec3 position = vec3::zero;
    quat rotation = quat::identity;
    int kills = 0;
    int deaths = 0;

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const PlayerJoin&) const = default;
  };

}
