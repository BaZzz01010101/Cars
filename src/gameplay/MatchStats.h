#pragma once
#include "PlayerStats.h"

namespace game
{
  typedef std::vector<PlayerStats>::iterator PlayerStatsIterator;

  struct MatchStats
  {
    std::vector<PlayerStats> playerStats;

    void addPlayer(uint64_t guid, int kills, int deaths);
    void removePlayer(uint64_t playerGuid);
    void addKill(uint64_t playerGuid);
    void addDeath(uint64_t playerGuid);
    void setPing(uint64_t playerGuid, int ping);
    PlayerStatsIterator getIterator(uint64_t playerGuid);
    PlayerStats* tryGetStats(uint64_t playerGuid);
    void updateOrder(PlayerStatsIterator playerStatsIt);
    void clear();
    void reset();
  };

}