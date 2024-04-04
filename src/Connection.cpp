#include "core.h"
#include "Connection.h"

namespace game
{

  void Connection::writePlayerState(const PlayerState& playerState)
  {
    static int bytesSent = 0;
    static high_resolution_clock clock {};
    static time_point tp = clock.now();

    nanoseconds duration = clock.now() - tp;

    if (duration > seconds(1))
    {
      printf("Bytes per second: %i\n", bytesSent);
      tp += seconds(1);
      bytesSent = 0;
    }

    playerStateOutQueue.tryPush(playerState);
    bytesSent += sizeof(playerState);
  }

  bool Connection::readPlayerState(PlayerState* playerState)
  {
    return playerStateInQueue.tryPop(playerState);
  }

}