#include "core.h"
#include "TurretState.h"
#include "MessageType.hpp"

namespace dto
{

  void TurretState::readFrom(BitStream& stream)
  {
    stream.Read(yaw);
    stream.Read(pitch);
  }

  void TurretState::writeTo(BitStream& stream) const
  {
    stream.Write(yaw);
    stream.Write(pitch);
  }

}