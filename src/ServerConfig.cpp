
#include "core.h"
#include "ServerConfig.h"

namespace game
{

  const ServerConfig ServerConfig::DEFAULT = {
    .host = "127.0.0.1",
    .port = 60000,
    .password = "^p3pSHwaj&uy59EU9T&H",
    .maxPlayers = 4,
  };

}