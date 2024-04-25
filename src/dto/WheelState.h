#pragma once

namespace dto
{
 
  struct WheelState
  {
    float suspensionOffset {};
    float suspensionSpeed {};
    float rotationSpeed {};

    void readFrom(BitStream& stream);
    void writeTo(BitStream& stream) const;
    bool operator==(const WheelState&) const = default;
  };

}