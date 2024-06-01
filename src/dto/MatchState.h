#pragma once

namespace dto
{
  struct MatchState
  {
    float matchTimeout {};
    bool shouldResetMatchStats {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const MatchState&) const = default;
  };

}