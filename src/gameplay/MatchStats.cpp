#include "core.h"
#include "MatchStats.h"

namespace game
{

  void MatchStats::addPlayer(uint64_t guid, int kills, int deaths)
  {
    PlayerStatsIterator it = getIterator(guid);

    if (it != playerStats.end())
    {
      it->kills = kills;
      it->deaths = deaths;
    }
    else
      playerStats.push_back({
        .guid = guid,
        .kills = kills,
        .deaths = deaths,
        .ping = 0
      });
  }

  void MatchStats::removePlayer(uint64_t playerGuid)
  {
    PlayerStatsIterator it = getIterator(playerGuid);

    if (it != playerStats.end())
      playerStats.erase(it);
  }

  void MatchStats::addKill(uint64_t playerGuid)
  {
    PlayerStatsIterator it = getIterator(playerGuid);

    if (it != playerStats.end())
    {
      it->kills++;
      updateOrder(it);
    }
  }

  void MatchStats::addDeath(uint64_t playerGuid)
  {
    PlayerStatsIterator it = getIterator(playerGuid);

    if (it != playerStats.end())
    {
      it->deaths++;
      updateOrder(it);
    }
  }

  void MatchStats::setPing(uint64_t playerGuid, int ping)
  {
    PlayerStatsIterator it = getIterator(playerGuid);

    if (it != playerStats.end())
      it->ping = ping;
  }

  std::vector<PlayerStats>::iterator MatchStats::getIterator(uint64_t playerGuid)
  {
    return std::find_if(playerStats.begin(), playerStats.end(), [=](const PlayerStats& ps) constexpr {
      return ps.guid == playerGuid;
    });
  }

  PlayerStats* MatchStats::tryGetStats(uint64_t playerGuid)
  {
    PlayerStatsIterator it = getIterator(playerGuid);

    return it != playerStats.end() ? &*it : nullptr;
  }

  void MatchStats::updateOrder(PlayerStatsIterator playerStatsIt)
  {
    for (PlayerStatsIterator it = playerStats.begin(); it != playerStatsIt; it++)
      if (it->getScore() < playerStatsIt->getScore())
      {
        PlayerStats ps = *playerStatsIt;
        playerStats.erase(playerStatsIt);
        playerStats.insert(it, ps);
        break;
      }
  }

  void MatchStats::clear()
  {
    playerStats.clear();
  }

  void MatchStats::reset()
  {
    for (PlayerStats& ps : playerStats)
    {
      ps.kills = 0;
      ps.deaths = 0;
    }
  }

}
