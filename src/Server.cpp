#include "core.h"
#include "Server.h"
#include <thread>
#include <chrono>
#include <iostream>

namespace network
{
  void Server::initialize()
  {
    std::thread t(&Server::listen, this);
    t.detach();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    connect();
  }

  void Server::connect()
  {
    // Создание экземпляра RakPeerInterface
    RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();

    // Инициализация сокета
    RakNet::SocketDescriptor sd;
    peer->Startup(1, &sd, 1);

    // Подключение к серверу
    std::string serverAddress = "127.0.0.1"; // Адрес сервера
    unsigned short serverPort = 60000; // Порт сервера
    RakNet::ConnectionAttemptResult result = peer->Connect(serverAddress.c_str(), serverPort, nullptr, 0);

    if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
    {
      std::cerr << "Failed to connect to the server" << std::endl;
      return;
    }

    std::cout << "Connected to the server" << std::endl;

    // Бесконечный цикл обработки сообщений
    while (true) 
    {
      //virtual uint32_t Send(const char* data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel, const AddressOrGUID systemIdentifier, bool broadcast, uint32_t forceReceiptNumber = 0) = 0;
      peer->Send("Hello, server!", (int)strlen("Hello, server!"), PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

      RakNet::Packet* packet = peer->Receive(); // Получение пакета

      if (packet)
      {
        switch (packet->data[0])
        {
          case ID_CONNECTION_REQUEST_ACCEPTED:
            printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
            break;
            // print out errors
          case ID_CONNECTION_ATTEMPT_FAILED:
            printf("Client Error: ID_CONNECTION_ATTEMPT_FAILED\n");
            break;
          case ID_ALREADY_CONNECTED:
            printf("Client Error: ID_ALREADY_CONNECTED\n");
            break;
          case ID_CONNECTION_BANNED:
            printf("Client Error: ID_CONNECTION_BANNED\n");
            break;
          case ID_INVALID_PASSWORD:
            printf("Client Error: ID_INVALID_PASSWORD\n");
            break;
          case ID_INCOMPATIBLE_PROTOCOL_VERSION:
            printf("Client Error: ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
            break;
          case ID_NO_FREE_INCOMING_CONNECTIONS:
            printf("Client Error: ID_NO_FREE_INCOMING_CONNECTIONS\n");
            break;
          case ID_DISCONNECTION_NOTIFICATION:
            //printf("ID_DISCONNECTION_NOTIFICATION\n");
            break;
          case ID_CONNECTION_LOST:
            printf("Client Error: ID_CONNECTION_LOST\n");
            break;
        }

        // Освобождение памяти, занятой пакетом
        peer->DeallocatePacket(packet);
      }
    }

    // Остановка и освобождение ресурсов
    peer->Shutdown(300);
    RakNet::RakPeerInterface::DestroyInstance(peer);
  }


  void Server::listen()
  {
    // Создание экземпляра RakPeerInterface
    RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();

    // Настройка параметров сервера
    RakNet::SocketDescriptor sd(60000, "127.0.0.1"); // Прослушиваемый порт
    peer->Startup(8, &sd, 1);
    peer->SetMaximumIncomingConnections(8);

    // Отображение сообщения о запуске сервера
    std::cout << "Server started. Waiting for connections..." << std::endl;

    // Бесконечный цикл прослушивания событий
    while (true)
    {
      RakNet::Packet* packet = peer->Receive(); // Получение пакета

      if (packet)
      {
        switch (packet->data[0])
        {
          case ID_NEW_INCOMING_CONNECTION:
            std::cout << "New client connected: " << packet->systemAddress.ToString() << std::endl;
            break;
          case ID_DISCONNECTION_NOTIFICATION:
            std::cout << "Client disconnected: " << packet->systemAddress.ToString() << std::endl;
            break;
          case ID_CONNECTION_LOST:
            std::cout << "Client lost connection: " << packet->systemAddress.ToString() << std::endl;
            break;
          default:
            std::cout << "Received a message with unknown ID: " << (int)packet->data[0] << std::endl;
            break;
        }

        // Освобождение памяти, занятой пакетом
        peer->DeallocatePacket(packet);
      }
    }

    // Остановка и освобождение ресурсов
    peer->Shutdown(300);
    RakNet::RakPeerInterface::DestroyInstance(peer);
  }
}