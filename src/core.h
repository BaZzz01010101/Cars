#pragma once

#define _USE_MATH_DEFINES
#define NOMINMAX

#include <algorithm>
#include <stdlib.h>
#include <cstdint>
#include <optional>
#include <memory>
#include <vector>
#include <list>
#include <string>
#include <cmath>
#include <array>
#include <functional>
#include <csignal>
#include <thread>
#include <chrono>
#include <cinttypes>
#include <numeric>
#include <format>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "quat.h"
#include "vec2.h"
#include "vec3.h"
#include "Sphere.h"

#if defined(_WIN32)           
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#define NOMINMAX         // Macros min(a,b) and max(a,b)
#endif

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "MessageType.hpp"

#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif

using namespace std::chrono;
using namespace RakNet;

namespace game
{
  static constexpr int MAX_PLAYER_NAME_LENGTH = 31;
  static constexpr int PLAYER_NAME_BUF_SIZE = MAX_PLAYER_NAME_LENGTH + 1;

  typedef std::array<char, PLAYER_NAME_BUF_SIZE> PlayerName;
  static constexpr PlayerName DEFAULT_PLAYER_NAME = { "UNKNOWN" };
}
