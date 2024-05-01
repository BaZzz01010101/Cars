#pragma once

namespace dto
{

  struct TurretState
  {
    float yaw {};
    float pitch {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const TurretState&) const = default;
  };

}

