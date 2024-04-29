#include "core.h"
#include "vec3.h"
#include "Helpers.h"

namespace game
{
  vec3 const vec3::zero = { 0, 0, 0 };

  vec3 const vec3::forward = { 0, 0, 1 };

  vec3 const vec3::up = { 0, 1, 0 };

  vec3 const vec3::left = { 1, 0, 0 };

  vec3 const vec3::min = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

  vec3 const vec3::max = { FLT_MAX, FLT_MAX, FLT_MAX };

  vec3 vec3::randomInSphere(float radius)
  {
    if (radius < EPSILON)
      return vec3::zero;

    vec3 v;

    do
    {
      v.x = randf(-radius, radius);
      v.y = randf(-radius, radius);
      v.z = randf(-radius, radius);
    } while (v.sqLength() >= radius * radius);

    return v;
  }

  vec3 vec3::randomOnSphereSurface(float radius)
  {
    return vec3::forward.rotatedBy(quat::random()) * radius;
  }

  vec3 vec3::randomInHollowSphere(float innerRadius, float outerRadius)
  {
    if (innerRadius < 0 || outerRadius <= 0 || innerRadius > outerRadius)
      return vec3::zero;

    if (innerRadius < EPSILON)
      return vec3::randomInSphere(outerRadius);

    float thickness = outerRadius - innerRadius;

    if (thickness < EPSILON)
      return vec3::randomOnSphereSurface(innerRadius);

    vec3 v;
    do v = vec3::randomInSphere(thickness);
    while (v.sqLength() >= thickness * thickness);

    return v + v.normalized() * innerRadius;
  }

  vec3 vec3::randomInCube(float size)
  {
    if (size <= 0)
      return vec3::zero;

    return
    {
      randf(-size, size),
      randf(-size, size),
      randf(-size, size),
    };
  }

  vec3 vec3::randomInBox(float xSize, float ySize, float zSize)
  {
    if (xSize <= 0)
      return vec3::zero;

    if (ySize <= 0)
      return vec3::zero;

    if (zSize <= 0)
      return vec3::zero;

    return
    {
      randf(-xSize, xSize),
      randf(-ySize, ySize),
      randf(-zSize, zSize),
    };
  }

  vec3 vec3::lerp(vec3 from, vec3 to, float factor)
  {
    return factor * to + (1 - factor) * from;
  }

  vec3::vec3(Vector3 v) : Vector3 { v }
  {
  }

  vec3::vec3(float x, float y, float z) : Vector3 { x, y, z }
  {
  }

  vec3 vec3::operator+(vec3 v) const
  {
    return { x + v.x, y + v.y, z + v.z };
  }

  vec3 vec3::operator-(vec3 v) const
  {
    return { x - v.x, y - v.y, z - v.z };
  }

  float vec3::operator*(vec3 v) const
  {
    return x * v.x + y * v.y + z * v.z;
  }

  vec3 vec3::operator%(vec3 v) const
  {
    return
    {
      y * v.z - z * v.y,
      z * v.x - x * v.z,
      x * v.y - y * v.x,
    };
  }

  vec3 vec3::operator+(float f) const
  {
    return { x + f, y + f, z + f };
  }

  vec3 vec3::operator-(float f) const
  {
    return { x - f, y - f, z - f };
  }

  vec3 vec3::operator*(float f) const
  {
    return { x * f, y * f, z * f };
  }

  vec3 vec3::operator/(float f) const
  {
    if (f == 0)
      return { 0, 0, 0 };

    return { x / f, y / f, z / f };
  }

  vec3& vec3::operator+=(vec3 v)
  {
    *this = *this + v;

    return *this;
  }

  vec3& vec3::operator-=(vec3 v)
  {
    *this = *this - v;

    return *this;
  }

  vec3& vec3::operator+=(float f)
  {
    *this = *this + f;

    return *this;
  }

  vec3& vec3::operator-=(float f)
  {
    *this = *this - f;

    return *this;
  }

  vec3& vec3::operator*=(float f)
  {
    *this = *this * f;

    return *this;
  }

  vec3& vec3::operator/=(float f)
  {
    *this = *this / f;

    return *this;
  }

  vec3 vec3::operator-() const
  {
    return { -x, -y, -z };
  }

  bool vec3::operator==(vec3 v) const
  {
    return x == v.x && y == v.y && z == v.z;
  }

  bool vec3::operator!=(vec3 v) const
  {
    return !(*this == v);
  }

  vec3::operator Vector3() const
  {
    return { x, y, z };
  }

  float vec3::sqLength() const
  {
    return x * x + y * y + z * z;
  }

  float vec3::length() const
  {
    float sqLen = sqLength();

    if (sqLen == 0)
      return 0;

    return sqrtf(sqLen);
  }

  vec3 vec3::normalized() const
  {
    float len = length();

    if (len == 0)
      return *this;

    return *this / len;
  }

  void vec3::normalize()
  {
    *this = normalized();
  }

  bool vec3::isNormalized() const
  {
    return fabsf(sqLength() - 1) < EPSILON;
  }

  bool vec3::isZero() const
  {
    return x == 0 && y == 0 && z == 0;
  }

  bool vec3::isAlmostZero(float delta) const
  {
    return
      fabsf(x) < delta &&
      fabsf(y) < delta &&
      fabsf(z) < delta;
  }

  vec3 vec3::reflected(vec3 normal) const
  {
    return *this - 2 * (*this * normal) * normal;
  }

  void vec3::reflect(vec3 normal)
  {
    *this = reflected(normal);
  }

  vec3 vec3::projectedOnVector(vec3 v) const
  {
    return *this * v * v;
  }

  void vec3::projectOnVector(vec3 v)
  {
    *this = projectedOnVector(v);
  }

  vec3 vec3::projectedOnPlane(vec3 normal) const
  {
    //return *this % normal % -normal;
    return *this - this->projectedOnVector(normal);
  }

  void vec3::projectOnPlane(vec3 normal)
  {
    *this = projectedOnPlane(normal);
  }

  vec3 vec3::rotatedBy(const quat& q) const
  {
    return Vector3RotateByQuaternion(*this, q);
  }

  void vec3::rotateBy(const quat& q)
  {
    *this = rotatedBy(q);
  }

  vec3 vec3::rotatedBy(vec3 axis, float angle) const
  {
    return Vector3RotateByAxisAngle(*this, axis, angle);
  }

  void vec3::rotateBy(vec3 axis, float angle)
  {
    *this = rotatedBy(axis, angle);
  }

  vec3 vec3::rotatedOnPlane(vec3 normal) const
  {
    return projectedOnPlane(normal).normalized() * length();
  }

  void vec3::rotateOnPlane(vec3 normal)
  {
    *this = rotatedOnPlane(normal);
  }

  vec3 operator*(float f, const vec3& v)
  {
    return v * f;
  }

  vec3 vec3::logarithmic() const
  {
    return{
      sign(x) * log(abs(x)),
      sign(y) * log(abs(y)),
      sign(z) * log(abs(z)),
    };
  }

  void vec3::zeroIfLessThen(float delta)
  {
    if (sqLength() < sqr(delta))
      *this = vec3::zero;
  }

  float vec3::getXAngle() const
  {
    if (y == 0 && z == 0)
      return 0;

    return atan2f(y, z);
  }

  float vec3::getYAngle() const
  {
    if (x == 0 && z == 0)
      return 0;

    return atan2f(x, z);
  }

  float vec3::getZAngle() const
  {
    if (x == 0 && y == 0)
      return 0;

    return atan2f(y, x);
  }

  void vec3::yawPitch(float* yaw, float* pitch) const
  {
    if (yaw)
      *yaw = atan2(x, z);

    if (pitch)
    {
      float distanceXY = sqrt(x * x + z * z);
      *pitch = atan2(-y, distanceXY);
    }
  }

  void vec3::yawPitchTo(vec3 target, float* yaw, float* pitch) const
  {
    if (yaw)
      *yaw = atan2(target.z - z, target.x - x);

    float distanceXY = sqrt((target.x - x) * (target.x - x) + (target.z - z) * (target.z - z));

    if (pitch)
      *pitch = atan2(target.y - y, distanceXY);
  }

  vec2 vec3::xy() const
  {
    return vec2 { x, y };
  }

  vec2 vec3::yz() const
  {
    return { y, z };
  }

  vec2 vec3::xz() const
  {
    return { x, z };
  }
}