#include "pch.h"
#include "vec2.h"
#include "Helpers.h"

namespace game
{
  const vec2 vec2::zero = { 0, 0 };
  const vec2 vec2::xAxis = { 1, 0 };
  const vec2 vec2::yAxis = { 0, 1 };

  vec2 vec2::randomInCircle(float radius)
  {
    vec2 v;
    const float RAND_MAX_2 = RAND_MAX / 2.0f;

    do
    {
      v.x = randf(-radius, radius);
      v.y = randf(-radius, radius);
    } while (v.sqLength() >= radius * radius);

    return v;
  }

  vec2 vec2::randomInSquare(float size)
  {
    return
    {
      randf(-size, size),
      randf(-size, size),
    };
  }

  vec2::vec2(Vector2 v) : Vector2{ v }
  {}

  vec2::vec2(float x, float y) : Vector2{ x, y }
  {}

  vec2 vec2::operator+(vec2 v) const
  {
    return { x + v.x, y + v.y };
  }

  vec2 vec2::operator-(vec2 v) const
  {
    return { x - v.x, y - v.y };
  }

  float vec2::operator*(vec2 v) const
  {
    return x * v.x + y * v.y;
  }

  vec2 vec2::operator*(float f) const
  {
    return { x * f, y * f };
  }

  vec2 vec2::operator/(float f) const
  {
    return { x / f, y / f };
  }

  vec2& vec2::operator+=(vec2 v)
  {
    x += v.x;
    y += v.y;

    return *this;
  }

  vec2& vec2::operator-=(vec2 v)
  {
    x -= v.x;
    y -= v.y;

    return *this;
  }

  vec2& vec2::operator*=(float f)
  {
    x *= f;
    y *= f;

    return *this;
  }

  vec2& vec2::operator/=(float f)
  {
    x /= f;
    y /= f;

    return *this;
  }

  vec2 vec2::operator-() const
  {
    return { -x, -y };
  }

  bool vec2::operator==(vec2 v) const
  {
    return x == v.x && y == v.y;
  }

  bool vec2::operator!=(vec2 v) const
  {
    return !(*this == v);
  }

  vec2::operator Vector2() const
  {
    return { x, y };
  }

  float vec2::sqLength() const
  {
    return x * x + y * y;
  }

  float vec2::length() const
  {
    float sqLen = sqLength();

    if (sqLen == 0)
      return 0;

    return sqrtf(sqLen);
  }

  vec2 vec2::normalized() const
  {
    float len = length();

    if (len == 0)
      return *this;

    return *this / len;
  }

  void vec2::normalize()
  {
    *this = normalized();
  }

  bool vec2::isZero() const
  {
    return x == 0 && y == 0;
  }

  bool vec2::isAlmostZero(float delta) const
  {
    return
      fabsf(x) <= delta &&
      fabsf(y) <= delta;
  }

  vec2 vec2::reflected(vec2 normal) const
  {
    return *this - 2 * (*this * normal) * normal;
  }

  void vec2::reflect(vec2 normal)
  {
    *this = reflected(normal);
  }

  vec2 vec2::projectedOnVector(vec2 v) const
  {
    return *this * v * v;
  }

  void vec2::projectOnVector(vec2 v)
  {
    *this = projectedOnVector(v);
  }

  vec2 vec2::logarithmic() const
  {
    return{
      sign(x) * log(abs(x)),
      sign(y) * log(abs(y)),
    };
  }

  void vec2::zeroIfLessThen(float delta)
  {
    if (sqLength() < sqr(delta))
      *this = vec2::zero;
  }

  float vec2::getAngle() const
  {
    return atan2(y, x);
  }

  vec2 operator*(float f, const vec2& v)
  {
    return v * f;
  }
}