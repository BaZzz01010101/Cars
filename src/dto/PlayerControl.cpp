#include "core.h"
#include "PlayerControl.h"
#include "MessageType.hpp"

namespace dto
{
  void PlayerControl::readFrom(BitStream& stream)
  {
    stream.Read(physicalFrame);
    stream.Read(guid);

    stream.Read(steeringAxis);
    stream.Read(accelerationAxis);
    stream.Read(thrustAxis);
    stream.Read(target);
    stream.Read(primaryFire);
    stream.Read(secondaryFire);
    stream.Read(handBrake);
  }

  void PlayerControl::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_CONTROL);
    stream.Write(physicalFrame);
    stream.Write(guid);

    stream.Write(steeringAxis);
    stream.Write(accelerationAxis);
    stream.Write(thrustAxis);
    stream.Write(target);
    stream.Write(primaryFire);
    stream.Write(secondaryFire);
    stream.Write(handBrake);
  }
}