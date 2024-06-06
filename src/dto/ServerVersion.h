#pragma once
#include "version.hpp"

namespace dto
{
  using namespace game;

  struct ServerVersion
  {
    Version version {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const ServerVersion&) const = default;
  };

}
