#include "core.h"
#include "PlayerStats.h"

namespace game
{
  int PlayerStats::getScore() const
  {
    return kills - deaths;
  }
}