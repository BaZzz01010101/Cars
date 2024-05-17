#include "core.h"
#include "PlayerState.h"
#include "MessageType.hpp"

namespace dto
{
  void PlayerState::readFrom(BitStream& stream)
  {
    stream.Read(physicalFrame);
    stream.Read(guid);

    stream.Read(ping);
    stream.Read(position);
    stream.Read(rotation);
    stream.Read(velocity);
    stream.Read(angularVelocity);
    stream.Read(steeringAngle);
    stream.Read(health);

    frontLeftWheelState.readFrom(stream);
    frontRightWheelState.readFrom(stream);
    rearLeftWheelState.readFrom(stream);
    rearRightWheelState.readFrom(stream);

    gunState.readFrom(stream);
    cannonState.readFrom(stream);
  }

  void PlayerState::writeTo(BitStream& stream) const
  {
    stream.Write(ID_PLAYER_STATE);
    stream.Write(physicalFrame);
    stream.Write(guid);

    stream.Write(ping);
    stream.Write(position);
    stream.Write(rotation);
    stream.Write(velocity);
    stream.Write(angularVelocity);
    stream.Write(steeringAngle);
    stream.Write(health);

    frontLeftWheelState.writeTo(stream);
    frontRightWheelState.writeTo(stream);
    rearLeftWheelState.writeTo(stream);
    rearRightWheelState.writeTo(stream);

    gunState.writeTo(stream);
    cannonState.writeTo(stream);
  }
}