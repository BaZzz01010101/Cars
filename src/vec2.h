#pragma once

namespace game
{

  struct vec2 : public Vector2
  {
    static const vec2 zero;
    static const vec2 xAxis;
    static const vec2 yAxis;
    static vec2 randomInCircle(float radius);
    static vec2 randomOnCircleSurface(float radius);
    static vec2 randomInRing(float innerRadius, float outerRadius);
    static vec2 randomInSquare(float size);

    vec2() = default;
    vec2(Vector2 v);
    vec2(float x, float y);

    vec2 operator+(vec2 v) const;
    vec2 operator-(vec2 v) const;
    float operator*(vec2 v) const;
    vec2 operator+(float f) const;
    vec2 operator-(float f) const;
    vec2 operator*(float f) const;
    vec2 operator/(float f) const;
    vec2& operator+=(vec2 v);
    vec2& operator-=(vec2 v);
    vec2& operator+=(float f);
    vec2& operator-=(float f);
    vec2& operator*=(float f);
    vec2& operator/=(float f);
    vec2 operator-() const;

    bool operator==(vec2 v) const;
    bool operator!=(vec2 v) const;

    operator Vector2() const;

    float sqLength() const;
    float length() const;
    vec2 normalized() const;
    void normalize();
    bool isZero() const;
    bool isAlmostZero(float delta) const;
    vec2 reflected(vec2 normal) const;
    void reflect(vec2 normal);
    vec2 projectedOnVector(vec2 v) const;
    void projectOnVector(vec2 v);
    vec2 right() const;
    vec2 logarithmic() const;
    void zeroIfLessThen(float delta);
    float getAngle() const;
    vec3 intoYZWithX(float x) const;
    vec3 intoXZWithY(float y) const;
    vec3 intoXYWithZ(float z) const;
  };

  vec2 operator*(float f, const vec2& v);

}
