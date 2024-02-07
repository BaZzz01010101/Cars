#include "pch.h"
#include "quat.h"
#include <Helpers.h>

namespace game
{

  const quat quat::identity = { 0, 0, 0, 1 };

  quat quat::fromXAngle(float angle)
  {
    return quat{ sinf(0.5f * angle), 0, 0, cosf(0.5f * angle) };
  }

  quat quat::fromYAngle(float angle)
  {
    return quat{ 0, sinf(0.5f * angle), 0, cosf(0.5f * angle) };
  }

  quat quat::fromZAngle(float angle)
  {
    return quat{ 0, 0, sinf(0.5f * angle), cosf(0.5f * angle) };
  }

  quat quat::fromAxisAngle(const vec3& axis, float angle)
  {
    return QuaternionFromAxisAngle(axis, angle);
  }

  quat quat::fromEuler(float yaw, float pitch, float roll)
  {
    return QuaternionFromEuler(roll, yaw, pitch);
  }

  quat quat::fromVectorToVector(vec3 v0, vec3 v1)
  {
    return QuaternionFromVector3ToVector3(v0, v1);
  }

  //quat::quat(const quat& q) :
  //  Quaternion{ q }
  //{}

  //quat::quat(const quat&& q) noexcept :
  //  Quaternion{ q }
  //{}

  quat::quat(Quaternion q) : Quaternion{ q }
  {}

  quat::quat(float x, float y, float z, float w) : Quaternion{ x, y, z, w }
  {}

  //quat& quat::operator=(const quat& q)
  //{
  //  x = q.x;
  //  y = q.y;
  //  z = q.z;
  //  w = q.w;

  //  return *this;
  //}

  //quat& quat::operator=(const quat&& q) noexcept
  //{
  //  x = q.x;
  //  y = q.y;
  //  z = q.z;
  //  w = q.w;

  //  return *this;
  //}

  quat quat::operator*(quat q) const
  {
    return QuaternionMultiply(*this, q);
  }

  quat quat::inverted()
  {
    return QuaternionInvert(*this);
  }

  void quat::invert()
  {
    *this = QuaternionInvert(*this);
  }

  quat::operator Quaternion() const
  {
    return { x, y, z, w };
  }

  quat quat::normalized()
  {
    return QuaternionNormalize(*this);
  }

  void quat::normalize()
  {
    *this = normalized();
  }

  quat quat::rotatedByXAngle(float angle) const
  {
    float sinha = sinf(0.5f * angle);
    float cosha = cosf(0.5f * angle);

    float nw = cosha * w - sinha * x;
    float nx = cosha * x + sinha * w;
    float ny = cosha * y - sinha * z;
    float nz = cosha * z + sinha * y;

    return quat{ nx, ny, nz, nw };
  }

  quat quat::rotatedByYAngle(float angle) const
  {
    float sinha = sinf(0.5f * angle);
    float cosha = cosf(0.5f * angle);

    float nw = cosha * w - sinha * y;
    float nx = cosha * x + sinha * z;
    float ny = cosha * y + sinha * w;
    float nz = cosha * z - sinha * x;

    return quat{ nx, ny, nz, nw };
  }

  quat quat::rotatedByZAngle(float angle) const
  {
    float sinha = sinf(0.5f * angle);
    float cosha = cosf(0.5f * angle);

    float nw = cosha * w + sinha * z;
    float nx = cosha * x + sinha * y;
    float ny = cosha * y - sinha * x;
    float nz = cosha * z - sinha * w;

    return quat{ nx, ny, nz, nw };
  }

  void quat::rotateByXAngle(float angle)
  {
    *this = rotatedByXAngle(angle);
  }

  void quat::rotateByYAngle(float angle)
  {
    *this = rotatedByYAngle(angle);
  }

  void quat::rotateByZAngle(float angle)
  {
    *this = rotatedByZAngle(angle);
  }

  void quat::toAxisAngle(vec3* axis, float* angle) const
  {
    QuaternionToAxisAngle(*this, axis, angle);
  }

  void quat::toEuler(float* yaw, float* pitch, float* roll) const
  {
    Vector3 euler = QuaternionToEuler(*this);

    if (yaw) *yaw = normalizeAngle(euler.y);
    if (pitch) *pitch = normalizeAngle(euler.z);
    if (roll) *roll = normalizeAngle(euler.x);
  }
}