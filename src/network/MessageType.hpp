#pragma once
#include "core.h"

namespace game
{

  enum : RakNet::MessageID
  {
    ID_PLAYER_CONTROL = ID_USER_PACKET_ENUM,
    ID_PLAYER_STATE,
    ID_PLAYER_HIT,
    ID_PLAYER_KILL,
    ID_PLAYER_JOIN,
    ID_PLAYER_LEAVE,
    ID_MATCH_STATE,
  };

}
