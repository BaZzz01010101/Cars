#include "core.h"
#include "ServerVersion.h"
#include "MessageType.hpp"

namespace dto
{

  void ServerVersion::readFrom(BitStream& stream)
  {
    stream.Read(version.major);
    stream.Read(version.minor);
    stream.Read(version.patch);
  }

  void ServerVersion::writeTo(BitStream& stream) const
  {
    stream.Write(ID_SERVER_VERSION);
    stream.Write(version.major);
    stream.Write(version.minor);
    stream.Write(version.patch);
  }

}