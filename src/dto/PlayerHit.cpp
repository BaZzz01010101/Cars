#include "core.h"
#include "PlayerHit.h"
#include "MessageType.hpp"

namespace dto
{
  void PlayerHit::readFrom(BitStream& stream)
  {
    stream.Read(physicalFrame);
    stream.Read(guid);

    stream.Read(attakerGuid);
    stream.Read(damage);

  }

  void PlayerHit::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_HIT);
    stream.Write(physicalFrame);
    stream.Write(guid);

    stream.Write(attakerGuid);
    stream.Write(damage);
  }
}