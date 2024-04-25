#include "core.h"
#include "PlayerJoin.h"

namespace dto
{

  void PlayerJoin::readFrom(BitStream& stream)
  {
    stream.Read(physicalFrame);
    stream.Read(guid);
    stream.Read(position);
    stream.Read(rotation);
  }

  void PlayerJoin::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_JOIN);
    stream.Write(physicalFrame);
    stream.Write(guid);
    stream.Write(position);
    stream.Write(rotation);
  }

}