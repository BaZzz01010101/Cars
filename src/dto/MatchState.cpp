#include "core.h"
#include "MatchState.h"
#include "MessageType.hpp"

namespace dto
{
  using namespace game;

  void MatchState::readFrom(BitStream& stream)
  {
    stream.Read(matchTimeout);
    stream.Read(shouldResetMatchStats);
  }

  void MatchState::writeTo(BitStream& stream) const
  {
    stream.Write(ID_MATCH_STATE);

    stream.Write(matchTimeout);
    stream.Write(shouldResetMatchStats);
  }

}