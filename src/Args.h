#pragma once
#include "ServerConfig.h"
#include "WindowConfig.h"

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
    WindowConfig windowConfig = WindowConfig::DEFAULT;
    ServerConfig serverConfig = ServerConfig::DEFAULT;

    Args(int argc, char** argv);
  };

}
