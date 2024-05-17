#pragma once

namespace game
{

  struct PlayerStats
  {
    uint64_t guid = 0;
    PlayerName name = DEFAULT_PLAYER_NAME;
    int kills = 0;
    int deaths = 0;
    int ping = 0;

    int getScore() const;
  };

}