#pragma once
#include "core.h"
#include "PlayerControl.h"
#include "PlayerState.h"
#include "PlayerJoin.h"
#include "PlayerLeave.h"

namespace network
{
  using namespace dto;

  class IServerMessageHandler
  {
  public:
    virtual inline void onClientConnected(uint64_t guid) = 0;
    virtual inline void onClientDisconnected(uint64_t guid) = 0;
    virtual inline void onPlayerControl(const PlayerControl& playerControl) = 0;
  };

}
