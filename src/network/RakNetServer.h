#pragma once
#include "Config.h"
#include "PlayerControl.h"
#include "PlayerState.h"
#include "IServerMessageHandler.h"

namespace network
{
  using namespace game;

  class RakNetServer
  {
  public:
    RakNetServer(const Config& config, IServerMessageHandler& messageHandler);
    virtual ~RakNetServer();

    void start();
    void stop();
    void disconnectClient(uint64_t guid, bool sendNotification);

    void broadcast(const BitStream& data, bool isReliable);
    void broadcastExcept(const BitStream& data, uint64_t guid, bool isReliable);
    void update();

  private:
    static constexpr int DISCONNECTION_TIMEOUT = 5;
    const Config& config;
    IServerMessageHandler& messageHandler;
    RakPeerInterface* peer = nullptr;
    Packet* packet = nullptr;
    std::unordered_map<uint64_t, time_point<steady_clock>> lastMessageTimes {};
    int maxConnections = 0;

    void dropHangConnections(time_point<steady_clock> now);
  };

}
