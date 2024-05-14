#include "core.h"
#include "PlayerKill.h"
#include "MessageType.hpp"

namespace dto
{
  void PlayerKill::readFrom(BitStream& stream)
  {
    stream.Read(physicalFrame);
    stream.Read(guid);

    stream.Read(killerGuid);
  }

  void PlayerKill::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_KILL);
    stream.Write(physicalFrame);
    stream.Write(guid);

    stream.Write(killerGuid);
  }
}