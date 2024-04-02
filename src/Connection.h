#pragma once
#include "PlayerState.h"
#include "Queue.hpp"

namespace game
{

  class Connection
  {
  public:
    void writePlayerState(const PlayerState& playerState);
    bool readPlayerState(PlayerState* playerState);

  private:
    static constexpr int PLAYER_STATE_IN_QUEUE_SIZE = 256;
    static constexpr int PLAYER_STATE_OUT_QUEUE_SIZE = 256;

    Queue<PlayerState, PLAYER_STATE_IN_QUEUE_SIZE> playerStateInQueue;
    Queue<PlayerState, PLAYER_STATE_OUT_QUEUE_SIZE> playerStateOutQueue;
  };

}