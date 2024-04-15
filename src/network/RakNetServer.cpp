#include "core.h"
#include "RakNetServer.h"
#include <iostream>

namespace network
{
  RakNetServer::RakNetServer(const Config& config, IServerMessageHandler& messageHandler) :
    config(config),
    messageHandler(messageHandler)
  {
    peer = RakNet::RakPeerInterface::GetInstance();
  }

  RakNetServer::~RakNetServer()
  {
    if (packet)
      peer->DeallocatePacket(packet);

    RakPeerInterface::DestroyInstance(peer);
  }

  void RakNetServer::start()
  {
    const char* serverAddress = config.multiplayer.serverAddress;
    unsigned short serverPort = config.multiplayer.serverPort;
    const char* password = config.multiplayer.serverPassword;
    int maxPlayers = config.multiplayer.maxPlayers;
    maxConnections = maxPlayers * 2;
    SocketDescriptor sd(serverPort, serverAddress);
    peer->Startup(maxConnections, &sd, 1);
    peer->SetMaximumIncomingConnections(maxConnections);
    peer->SetIncomingPassword(password, (int)strlen(password));

    printf("SERVER: Started: %s:%i. Waiting for connections...\n", serverAddress, serverPort);
  }

  void RakNetServer::stop()
  {
    peer->Shutdown(300);
    printf("SERVER: Stoped\n");
  }

  void RakNetServer::disconnectClient(uint64_t guid, bool sendNotification)
  {
    printf("SERVER: Disconnect client: %" PRIu64 "\n", guid);
    peer->CloseConnection(peer->GetSystemAddressFromGuid(RakNet::RakNetGUID(guid)), sendNotification, 0, PacketPriority::LOW_PRIORITY);
  }

  void RakNetServer::broadcast(const BitStream& data, bool isReliable)
  {
    //printf("SERVER: Broadcast packet: %i\n", data.GetData()[0]);
    PacketReliability reliability = isReliable ? PacketReliability::RELIABLE_ORDERED : PacketReliability::UNRELIABLE_SEQUENCED;
    peer->Send(&data, PacketPriority::MEDIUM_PRIORITY, reliability, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
  }

  void RakNetServer::broadcastExcept(const BitStream& data, uint64_t guid, bool isReliable)
  {
    //printf("SERVER: Broadcast packet: %i\n", data.GetData()[0]);
    SystemAddress exceptAddress = peer->GetSystemAddressFromGuid(RakNet::RakNetGUID(guid));
    PacketReliability reliability = isReliable ? PacketReliability::RELIABLE_ORDERED : PacketReliability::UNRELIABLE_SEQUENCED;
    peer->Send(&data, PacketPriority::MEDIUM_PRIORITY, reliability, 0, exceptAddress, true);
  }

  void RakNetServer::update()
  {
    time_point<steady_clock> now = steady_clock::now();
    dropHangConnections(now);

    while (Packet* packet = peer->Receive())
    {
      uint64_t senderGuid = packet->guid.g;
      lastMessageTimes[senderGuid] = now;
      MessageID type = packet->data[0];
      BitStream stream(packet->data, packet->length, false);
      stream.IgnoreBits(CHAR_BIT);
      //printf("SERVER: Receive packet type: %i\n", (int)type);

      switch (type)
      {
        case ID_NEW_INCOMING_CONNECTION:
        {
          printf("SERVER: Connected client: %s\n", packet->guid.ToString());
          messageHandler.onClientConnected(senderGuid);
          break;
        }
        case ID_DISCONNECTION_NOTIFICATION:
          printf("SERVER: Disconnected client: %s\n", packet->guid.ToString());
          messageHandler.onClientDisconnected(senderGuid);
          lastMessageTimes.erase(packet->guid.g);
          break;

        case ID_CONNECTION_LOST:
          printf("SERVER: Connection lost: %s\n", packet->guid.ToString());
          messageHandler.onClientDisconnected(senderGuid);
          lastMessageTimes.erase(packet->guid.g);
          break;

        case ID_PLAYER_CONTROL:
        {
          PlayerControl playerControl;
          playerControl.readFrom(stream);
          stream.ResetReadPointer();
          broadcastExcept(stream, senderGuid, false);
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

        default:
          printf("SERVER: Unknown packet: %i\n", packet->data[0]);
          break;
      }

      peer->DeallocatePacket(packet);
    }
  }

  void RakNetServer::dropHangConnections(time_point<steady_clock> now)
  {
    auto it = lastMessageTimes.begin();

    while(it != lastMessageTimes.end())
    {
      auto [guid, time] = *it;

      if (now - time > seconds(DISCONNECTION_TIMEOUT))
      {
        auto toErase = it++;
        lastMessageTimes.erase(toErase);
        disconnectClient(guid, false);
      }
      else
        ++it;
    }
  }
}