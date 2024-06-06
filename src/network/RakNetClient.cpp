#include "core.h"
#include "RakNetClient.h"
#include "PlayerControl.h"
#include "PlayerState.h"
#include "PlayerJoin.h"

using namespace game;
using namespace dto;

namespace network
{
  RakNetClient::RakNetClient(const ServerConfig& config, IClientMessageHandler& messageHandler) :
    config(config),
    messageHandler(messageHandler)
  {
    peer = RakNet::RakPeerInterface::GetInstance();
  }

  RakNetClient::~RakNetClient()
  {
    RakPeerInterface::DestroyInstance(peer);
  }

  bool RakNetClient::isConnected() const
  {
    return peer && peer->GetConnectionState(serverId) == ConnectionState::IS_CONNECTED;
  }

  bool RakNetClient::connect()
  {
    const char* serverAddress = config.host;
    unsigned short serverPort = config.port;
    const char* password = config.password;

    printf("CLIENT: Connecting to: %s:%i\n", serverAddress, serverPort);

    SocketDescriptor sd;
    peer->Startup(1, &sd, 1);
    resetTimeouts();

    ConnectionAttemptResult result = peer->Connect(serverAddress, serverPort, password, (int)strlen(password));

    if (result != CONNECTION_ATTEMPT_STARTED)
    {
      switch (result)
      {
        case INVALID_PARAMETER:
          printf("CLIENT: Failed to connect (INVALID_PARAMETER)\n");
          break;
        case CANNOT_RESOLVE_DOMAIN_NAME:
          printf("CLIENT: Failed to connect (CANNOT_RESOLVE_DOMAIN_NAME)\n");
          break;
        case ALREADY_CONNECTED_TO_ENDPOINT:
          printf("CLIENT: Failed to connect (ALREADY_CONNECTED_TO_ENDPOINT)\n");
          break;
        case CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS:
          printf("CLIENT: Failed to connect (CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS)\n");
          break;
        case SECURITY_INITIALIZATION_FAILED:
          printf("CLIENT: Failed to connect (SECURITY_INITIALIZATION_FAILED)\n");
          break;
      }

      return false;
    }

    time_point timeout = steady_clock::now() + seconds(CONNECTION_TIMEOUT);

    while (steady_clock::now() < timeout)
    {
      Packet* packet = peer->Receive();

      if (!packet)
        continue;

      MessageID type = packet->data[0];

      switch (type)
      {
        case ID_CONNECTION_REQUEST_ACCEPTED:
          serverId = packet->guid;
          messageHandler.onConnected(peer->GetMyGUID().g);
          printf("CLIENT: Connection accepted, server id: %s\n", serverId.ToString());
          return true;

        case ID_CONNECTION_ATTEMPT_FAILED:
          printf("CLIENT: Failed to connect. Connection attempt failed.\n");
          return false;

        case ID_ALREADY_CONNECTED:
          printf("CLIENT: Failed to connect. Already connected.\n");
          return false;

        case ID_NEW_INCOMING_CONNECTION:
          printf("CLIENT: Failed to connect. The server is not accepting new connections.\n");
          return false;

        case ID_INVALID_PASSWORD:
          printf("CLIENT: Failed to connect. Invalid passwordþ\n");
          return false;

        case ID_INCOMPATIBLE_PROTOCOL_VERSION:
          printf("CLIENT: Failed to connect. Incompatible protocol version.\n");
          return false;

        case ID_NO_FREE_INCOMING_CONNECTIONS:
          printf("CLIENT: Failed to connect. No free incoming connections.\n");
          return false;

        case ID_CONNECTION_BANNED:
          printf("CLIENT: Failed to connect. Connection banned.\n");
          return false;

        default:
          printf("CLIENT: Unknown message: %i\n", packet->data[0]);
          break;
      }

      std::this_thread::sleep_for(milliseconds(1));
    }

    printf("CLIENT: Failed to connect. Connection timeout.\n");
    return false;
  }

  void RakNetClient::disconnect()
  {
    peer->Shutdown(300);
    printf("CLIENT: Disconnected\n");
  }

  void RakNetClient::send(const BitStream& data)
  {
    //printf("CLIENT: Send packet type: %i\n", data.GetData()[0]);
    peer->Send(&data, PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0, serverId, false);
  }

  void RakNetClient::update()
  {
    MessageID type;
    updateNetworkIssuesTimeout();

    while (Packet* packet = peer->Receive())
    {
      networkIssuesTimeout = NETWORK_ISSUES_TIMEOUT;
      BitStream stream(packet->data, packet->length, false);
      stream.Read(type);
      //printf("CLIENT: Receive packet type: %i\n", (int)type);

      switch (type)
      {
        case ID_DISCONNECTION_NOTIFICATION:
        case ID_CONNECTION_LOST:
          messageHandler.onDisconnected(packet->guid.g);
          break;

        case ID_PLAYER_JOIN:
        {
          PlayerJoin playerJoin;
          playerJoin.readFrom(stream);
          messageHandler.onPlayerJoin(playerJoin);
          break;
        }

        case ID_PLAYER_LEAVE:
        {
          PlayerLeave playerLeave;
          playerLeave.readFrom(stream);
          messageHandler.onPlayerLeave(playerLeave);
          break;
        }

        case ID_PLAYER_CONTROL:
        {
          PlayerControl playerControl;
          playerControl.readFrom(stream);
          messageHandler.onPlayerControl(playerControl);
          break;
        }

        case ID_PLAYER_STATE:
        {
          PlayerState playerState;
          playerState.readFrom(stream);
          messageHandler.onPlayerState(playerState);
        }
        break;

        case ID_PLAYER_HIT:
        {
          PlayerHit playerHit;
          playerHit.readFrom(stream);
          messageHandler.onPlayerHit(playerHit);
        }
        break;

        case ID_PLAYER_KILL:
        {
          PlayerKill playerKill;
          playerKill.readFrom(stream);
          messageHandler.onPlayerKill(playerKill);
        }
        break;

        case ID_MATCH_STATE:
        {
          MatchState matchState;
          matchState.readFrom(stream);
          messageHandler.onMatchState(matchState);
          break;
        }

        case ID_SERVER_VERSION:
        {
          ServerVersion serverVersion;
          serverVersion.readFrom(stream);
          messageHandler.onServerVersion(serverVersion);
          break;
        }

        default:
          printf("CLIENT: Unknown message: %i\n", type);
          break;
      }

      if (isConnected())
        peer->DeallocatePacket(packet);
    }
  }

  void RakNetClient::resetTimeouts()
  {
    networkIssuesTimeout = NETWORK_ISSUES_TIMEOUT;
    networkIssuesCooldown = 0;
  }

  void RakNetClient::updateNetworkIssuesTimeout()
  {
    static std::chrono::steady_clock clock;
    static time_point lastTime = clock.now();

    time_point currentTime = clock.now();
    float elapsed = duration_cast<microseconds>(clock.now() - lastTime).count() / 1000000.0f;
    lastTime = currentTime;

    networkIssuesTimeout = std::max(0.0f, networkIssuesTimeout - elapsed);
    networkIssuesCooldown = std::max(0.0f, networkIssuesCooldown - elapsed);

    if (networkIssuesTimeout == 0)
      networkIssuesCooldown = NETWORK_ISSUES_COOLDOWN;
  }

}
