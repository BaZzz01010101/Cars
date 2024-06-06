#pragma once
#include "core.h"

namespace game
{

  struct Version
  {
    uint8_t major;
    uint8_t minor;
    uint32_t patch;
    bool operator==(const Version&) const = default;
    std::string toString() const { return std::format("{}.{}.{}", major, minor, patch); }
  };

  static constexpr Version VERSION { 1, 0, 0 };

}