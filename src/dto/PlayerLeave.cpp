#include "core.h"
#include "PlayerLeave.h"

namespace dto
{

  void PlayerLeave::readFrom(BitStream& stream)
  {
    stream.Read(guid);
  }

  void PlayerLeave::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_LEAVE);
    stream.Write(guid);
  }

}