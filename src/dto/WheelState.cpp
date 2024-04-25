#include "core.h"
#include "WheelState.h"
#include "MessageType.hpp"

namespace dto
{
  void WheelState::readFrom(BitStream& stream)
  {
    stream.Read(suspensionOffset);
    stream.Read(suspensionSpeed);
    stream.Read(rotationSpeed);
  }

  void WheelState::writeTo(BitStream& stream) const
  {
    stream.Write(suspensionOffset);
    stream.Write(suspensionSpeed);
    stream.Write(rotationSpeed);
  }
}