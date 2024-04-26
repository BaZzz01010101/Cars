#pragma once
#include "ServerConfig.h"
#include "IClientMessageHandler.h"
#include "PlayerControl.h"
#include "PlayerState.h"

namespace network
{
  using namespace game;

  class RakNetClient
  {
  public:
    RakNetClient(const ServerConfig& config, IClientMessageHandler& messageHandler);
    virtual ~RakNetClient();

    bool isConnected() const;

    bool connect();
    void disconnect();

    void send(const BitStream& data);
    void update();

  private:
    static constexpr int CONNECTION_TIMEOUT = 5;

    const ServerConfig& config;
    IClientMessageHandler& messageHandler;
    RakPeerInterface* peer = nullptr;
    Packet* packet = nullptr;
    RakNetGUID serverId = UNASSIGNED_RAKNET_GUID;
    RakNetGUID clientId = UNASSIGNED_RAKNET_GUID;
  };

}