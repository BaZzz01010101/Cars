#pragma once
#include "Config.h"
#include "Scene.h"
#include "PlayerState.h"
#include "Queue.hpp"
#include "RakNetServer.h"
#include "IServerMessageHandler.h"

namespace game
{
  using namespace network;
  using namespace dto;

  struct ServerApp : public IServerMessageHandler
  {
    static constexpr float SYNC_FACTOR = 0.5f;
    volatile bool exit;

    Config config {};
    Scene scene;
    RakNetServer network;
    high_resolution_clock clock {};

    ServerApp();

    void initialize();
    void run();
    void shutdown();

    void sendPlayerStates();

    virtual void onClientConnected(uint64_t guid) override;
    virtual void onClientDisconnected(uint64_t guid) override;
    virtual void onPlayerControl(const PlayerControl& playerControl) override;
    virtual void onPlayerState(const PlayerState& playerState) override;
  };

}