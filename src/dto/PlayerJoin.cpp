#include "core.h"
#include "PlayerJoin.h"

namespace dto
{

  void PlayerJoin::readFrom(BitStream& stream)
  {
    stream.Read(guid);
    stream.Read(position.x);
    stream.Read(position.y);
    stream.Read(position.z);
    stream.Read(rotation.x);
    stream.Read(rotation.y);
    stream.Read(rotation.z);
    stream.Read(rotation.w);
  }

  void PlayerJoin::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_JOIN);
    stream.Write(guid);
    stream.Write(position.x);
    stream.Write(position.y);
    stream.Write(position.z);
    stream.Write(rotation.x);
    stream.Write(rotation.y);
    stream.Write(rotation.z);
    stream.Write(rotation.w);
  }

}