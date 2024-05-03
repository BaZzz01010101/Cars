#pragma once
#include <functional>

namespace game
{
  extern int gSeed;
  const int FAST_RAND_MAX = 0x7fffffff;

  template<typename T1, typename T2>
  T1 clamp(T1 val, T2 min, T2 max)
  {
    return std::min(std::max(val, min), max);
  }

  template <typename T>
  inline T sign(T val)
  {
    return T((val > 0) - (val < 0));
  }

  template<class T>
  T sqr(T val)
  {
    return val * val;
  }

  template<class T>
  T sqrSigned(T val)
  {
    T result = val * val;
    return sign(val) * result;
  }

  template<class T>
  T lerp(T from, T to, float factor)
  {
    return static_cast<T>((1 - factor) * from + factor * to);
  }

  template <typename T>
  constexpr T sqrt_helper(T x, T lo, T hi)
  {
    if (lo == hi)
      return lo;

    const T mid = (lo + hi + 1) / 2;

    if (x / mid < mid)
      return sqrt_helper<T>(x, lo, mid - 1);
    else
      return sqrt_helper<T>(x, mid, hi);
  }

  template <typename T>
  constexpr T ct_sqrt(T x)
  {
    return sqrt_helper<T>(x, 0, x / 2 + 1);
  }

  // min2 can be greater than max2
  // min1 can be greater than max1
  // min1 can be equal to max1
  float mapRangeClamped(float value, float min1, float max1, float min2, float max2);

  vec2 moveTo(vec2 current, vec2 target, float maxDelta);
  vec3 moveTo(vec3 current, vec3 target, float maxDelta);
  float moveTo(float current, float target, float maxDelta);

  vec2 moveToRelative(vec2 current, vec2 target, float maxDeltaRelative);
  vec3 moveToRelative(vec3 current, vec3 target, float maxDeltaRelative);
  float moveToRelative(float current, float target, float maxDeltaRelative);
  float moveAngleToRelative(float current, float target, float maxDeltaRelative);

  float moveAngleTo(float current, float target, float maxDelta);

  int fastRand(void);

  inline int randi(int max)
  {
    return int((long long int)(max) * fastRand() / FAST_RAND_MAX);
  }

  inline int randi(int min, int max)
  {
    return min + randi(max - min);
  }

  inline float randf(float max)
  {
    return float(double(max) * fastRand() / FAST_RAND_MAX);
  }

  inline float randf(float min, float max)
  {
    return min + randf(max - min);
  }

  inline float normalizeAngle(float a)
  {
    while (a > PI)
      a -= 2 * PI;
    while (a < -PI)
      a += 2 * PI;

    return a;
  }

  inline void drawVector(vec3 pos, vec3 vec, Color color)
  {
    DrawLine3D(pos, pos + vec, color);
    DrawSphere(pos + vec, 0.1f, color);
  }

}