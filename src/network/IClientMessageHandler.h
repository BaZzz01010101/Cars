#pragma once
#include "core.h"
#include "PlayerControl.h"
#include "PlayerState.h"
#include "PlayerJoin.h"
#include "PlayerLeave.h"
#include "PlayerHit.h"
#include "PlayerKill.h"
#include "MatchState.h"

namespace network
{
  using namespace dto;

  class IClientMessageHandler
  {
  public:
    virtual void onConnected(uint64_t guid) = 0;
    virtual void onDisconnected(uint64_t guid) = 0;
    virtual void onPlayerJoin(const PlayerJoin& playerJoin) = 0;
    virtual void onPlayerLeave(const PlayerLeave& playerLeave) = 0;
    virtual void onPlayerControl(const PlayerControl& playerControl) = 0;
    virtual void onPlayerState(const PlayerState& playerState) = 0;
    virtual void onPlayerHit(const PlayerHit& playerHit) = 0;
    virtual void onPlayerKill(const PlayerKill& playerKill) = 0;
    virtual void onMatchState(const MatchState& matchState) = 0;
  };

}
