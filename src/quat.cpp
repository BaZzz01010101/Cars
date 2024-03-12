#include "pch.h"
#include "quat.h"
#include "Helpers.h"

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

  quat quat::random()
  {
    return quat::fromEuler(randf(2 * PI), randf(2 * PI), randf(2 * PI));

    float x, y, z, w;

    do
    {
      x = randf(-1, 1);
      y = randf(-1, 1);
      z = randf(-1, 1);
      w = randf(-1, 1);
    } while (x * x + y * y + z * z + w * w > 1);

    return quat{ x, y, z, w }.normalized();
  }

  quat::quat(Quaternion q) : Quaternion{ q }
  {}

  quat::quat(float x, float y, float z, float w) : Quaternion{ x, y, z, w }
  {}

  quat quat::operator*(quat q) const
  {
    return QuaternionMultiply(*this, q);
  }

  quat quat::inverted() const
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

  quat quat::normalized() const
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
    // Calculate euler angles from quaternion
    float sqw = w * w;
    float sqx = x * x;
    float sqy = y * y;
    float sqz = z * z;

    // Yaw (heading) calculation
    float t0 = 2.0f * (w * x + y * z);
    float t1 = 1.0f - 2.0f * (sqx + sqy);
    float t2 = atan2f(t0, t1);

    // Pitch (elevation) calculation
    float t3 = 2.0f * (w * y - z * x);
    t3 = t3 > 1.0f ? 1.0f : t3;
    t3 = t3 < -1.0f ? -1.0f : t3;
    float t4 = asinf(t3);

    // Roll (bank) calculation
    float t5 = 2.0f * (w * z + x * y);
    float t6 = 1.0f - 2.0f * (sqy + sqz);
    float t7 = atan2f(t5, t6);

    // Assign values to output variables
    if (yaw) *yaw = t2;
    if (pitch) *pitch = t4;
    if (roll) *roll = t7;
  }
}