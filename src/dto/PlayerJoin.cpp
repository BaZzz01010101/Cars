#include "core.h"
#include "PlayerJoin.h"

namespace dto
{

  void PlayerJoin::readFrom(BitStream& stream)
  {
    stream.Read(physicalFrame);
    stream.Read(guid);
    stream.Read(name);
    stream.Read(position);
    stream.Read(rotation);
    stream.Read(kills);
    stream.Read(deaths);
  }

  void PlayerJoin::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_JOIN);
    stream.Write(physicalFrame);
    stream.Write(guid);
    stream.Write(name);
    stream.Write(position);
    stream.Write(rotation);
    stream.Write(kills);
    stream.Write(deaths);
  }

}