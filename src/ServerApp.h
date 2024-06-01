#pragma once
#include "Config.h"
#include "ServerConfig.h"
#include "Scene.h"
#include "PlayerState.h"
#include "Queue.hpp"
#include "RakNetServer.h"
#include "IServerMessageHandler.h"
#include "MatchStats.h"

namespace game
{
  using namespace network;
  using namespace dto;

  struct ServerApp : public IServerMessageHandler
  {
    static constexpr PlayerName PLAYER_NAMES[] = {
      { "Elint Ceastwood" },
      { "Lruce Bee" },
      { "Nhuck Corris" },
      { "Cackie Jhan" },
      { "Srnold Achwarzenegger" },
      { "Let Ji" },
      { "Wenzel Dashington" },
      { "Com Truise" },
      { "Sill Wmith" },
      { "Reanu Keeves" },
    };

    static constexpr float SYNC_FACTOR = 0.5f;
    volatile bool exit;

    Config config {};
    Scene scene;
    RakNetServer network;
    high_resolution_clock clock {};
    nanoseconds maxSleep {};
    time_point<steady_clock> lastUpdateTime {};
    MatchStats matchStats {};

    int maxPlayers = 0;

    ServerApp(const Config& config, const ServerConfig& serverConfig);

    void initialize();
    bool pulse();
    void shutdown();

    void sendPlayerStates();
    void sendPlayerHits();
    void sendPlayerKills();
    void updateMatchRestart();
    void sendMatchState(uint64_t guid, bool resetMatchStats);
    void broadcastMatchState(bool resetMatchStats);

    PlayerName getRandomPlayerName();

    virtual void onClientConnected(uint64_t guid) override;
    virtual void onClientDisconnected(uint64_t guid) override;
    virtual void onPlayerControl(const PlayerControl& playerControl) override;
  };

}