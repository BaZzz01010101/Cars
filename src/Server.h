#pragma once
#include "RakPeerInterface.h"

namespace network 
{

  class Server
  {
  public:
    //Server();
    //~Server();

    void initialize();
    void connect();
    void listen();
    //void run();
    //void shutdown();
  private:
    RakNet::RakPeerInterface* peer;
  };

}
