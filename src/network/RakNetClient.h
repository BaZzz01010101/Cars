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
    bool isNetworkIssues() const { return networkIssuesCooldown > 0; }

  private:
    static constexpr int CONNECTION_TIMEOUT = 10;
    static constexpr float NETWORK_ISSUES_TIMEOUT = 0.2f;
    static constexpr float NETWORK_ISSUES_COOLDOWN = 3.0f;

    const ServerConfig& config;
    IClientMessageHandler& messageHandler;
    RakPeerInterface* peer = nullptr;
    Packet* packet = nullptr;
    float networkIssuesTimeout = NETWORK_ISSUES_TIMEOUT;
    float networkIssuesCooldown = 0;
    RakNetGUID serverId = UNASSIGNED_RAKNET_GUID;
    RakNetGUID clientId = UNASSIGNED_RAKNET_GUID;

    void resetTimeouts();
    void updateNetworkIssuesTimeout();
  };

}