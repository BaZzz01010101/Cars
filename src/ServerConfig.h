#pragma once
namespace game
{

  struct ServerConfig
  {
    const char* host {};
    unsigned short port {};
    const char* password {};
    int maxPlayers {};

    static const ServerConfig DEFAULT;
  };

}
