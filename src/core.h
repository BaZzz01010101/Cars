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

#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif

using namespace std::chrono;
