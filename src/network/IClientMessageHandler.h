#pragma once
#include "core.h"
#include "PlayerControl.h"
#include "PlayerState.h"
#include "PlayerJoin.h"
#include "PlayerLeave.h"

namespace network
{
  using namespace dto;

  class IClientMessageHandler
  {
  public:
    virtual inline void onConnected(uint64_t guid) = 0;
    virtual inline void onDisconnected(uint64_t guid) = 0;
    virtual inline void onPlayerJoin(const PlayerJoin& playerJoin) = 0;
    virtual inline void onPlayerLeave(const PlayerLeave& playerLeave) = 0;
    virtual inline void onPlayerControl(const PlayerControl& playerControl) = 0;
    virtual inline void onPlayerState(const PlayerState& playerState) = 0;
  };

}
