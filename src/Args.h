#pragma once
#include "ServerConfig.h"

namespace game
{

  class Args
  {
  private:
    int argc = 0;
    char** argv;

  public:
    enum Mode
    {
      Client,
      Server,
      Test
    };

    Mode mode = Mode::Client;
    ServerConfig serverConfig = ServerConfig::DEFAULT;

    Args(int argc, char** argv);
  };

}
