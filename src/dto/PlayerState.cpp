#include "core.h"
#include "PlayerState.h"
#include "MessageType.hpp"

namespace dto
{
  void PlayerState::readFrom(BitStream& stream)
  {
    stream.Read(guid);
    stream.Read(position);
    stream.Read(rotation);
    stream.Read(velocity);
    stream.Read(angularVelocity);
    stream.Read(gunYaw);
    stream.Read(gunPitch);
    stream.Read(cannonYaw);
    stream.Read(cannonPitch);
  }

  void PlayerState::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_STATE);
    stream.Write(guid);
    stream.Write(position);
    stream.Write(rotation);
    stream.Write(velocity);
    stream.Write(angularVelocity);
    stream.Write(gunYaw);
    stream.Write(gunPitch);
    stream.Write(cannonYaw);
    stream.Write(cannonPitch);
  }
}