#pragma once

namespace game
{
  struct quat;

  struct vec3 : public Vector3
  {
    static const vec3 zero;
    static const vec3 forward;
    static const vec3 up;
    static const vec3 left;
    static vec3 randomInSphere(float radius);
    static vec3 randomInCube(float size);

    vec3() = default;
    vec3(Vector3 v);
    vec3(float x, float y, float z);

    vec3 operator+(vec3 v) const;
    vec3 operator-(vec3 v) const;
    float operator*(vec3 v) const;
    vec3 operator%(vec3 v) const;
    vec3 operator*(float f) const;
    vec3 operator/(float f) const;
    vec3& operator+=(vec3 v);
    vec3& operator-=(vec3 v);
    vec3& operator*=(float v);
    vec3& operator/=(float v);
    vec3 operator-() const;

    bool operator==(vec3 v) const;
    bool operator!=(vec3 v) const;

    operator Vector3() const;

    float sqLength() const;
    float length() const;
    vec3 normalized() const;
    void normalize();
    bool isZero() const;
    bool isAlmostZero(float delta) const;
    vec3 reflected(vec3 normal) const;
    void reflect(vec3 normal);
    vec3 projectedOnVector(vec3 v) const;
    void projectOnVector(vec3 v);
    vec3 projectedOnPlane(vec3 normal) const;
    void projectOnPlane(vec3 normal);
    vec3 rotatedBy(const quat& q) const;
    void rotateBy(const quat& q);
    vec3 rotatedBy(vec3 axis, float angle) const;
    void rotateBy(vec3 axis, float angle);
    vec3 rotatedOnPlane(vec3 normal) const;
    void rotateOnPlane(vec3 normal);
    vec3 logarithmic() const;
    void zeroIfLessThen(float delta);
    float getXAngle() const;
    float getYAngle() const;
    float getZAngle() const;
  };

  vec3 operator*(float f, const vec3& v);

}
