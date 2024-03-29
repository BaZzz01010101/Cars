#pragma once
#include <functional>

namespace game
{
  extern int gSeed;
  const int FAST_RAND_MAX = 0x7fffffff;

  template<class T>
  T clamp(T val, T min, T max)
  {
    return std::min(std::max(val, min), max);
  }

  template<class T>
  T sqr(T val)
  {
    return val * val;
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

  vec3 moveTo(vec3 current, vec3 target, float maxDelta);

  float moveTo(float current, float target, float maxDelta);

  float moveAngleTo(float current, float target, float maxDelta);

  template <typename T>
  inline T sign(T val)
  {
    return T((val > 0) - (val < 0));
  }

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