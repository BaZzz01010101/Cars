#pragma once

namespace game
{
  static int gSeed = 0;
  const int FAST_RAND_MAX = 0x7fff;

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
    return max * fastRand() / FAST_RAND_MAX;
  }

  inline int randi(int min, int max)
  {
    return min + randi(max - min);
  }

  inline float randf(float max)
  {
    return max * fastRand() / FAST_RAND_MAX;
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

  //quadratic extrapolation
  inline vec3 extrapolate(vec3 v0, vec3 v1, vec3 v2, float k)
    {
      vec3 d1 = v1 - v0;
      vec3 d2 = v2 - v1;
      vec3 d3 = d2 - d1;

      return v2 + k * d2 + 0*d3 / 2;
    }

  inline void drawVector(vec3 pos, vec3 vec, Color color)
  {
    DrawLine3D(pos, pos + vec, color);
    DrawSphere(pos + vec, 0.1f, color);
  }
}