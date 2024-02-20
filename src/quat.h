#pragma once

namespace game
{
  struct vec3;

  struct quat : public Quaternion
  {
    static const quat identity;
    static quat fromXAngle(float angle);
    static quat fromYAngle(float angle);
    static quat fromZAngle(float angle);
    static quat fromAxisAngle(const vec3& axis, float angle);
    static quat fromEuler(float yaw, float pitch, float roll);
    static quat fromVectorToVector(vec3 v0, vec3 v1);
    quat() = default;
    quat(Quaternion q);
    quat(float x, float y, float z, float w);
    operator Quaternion() const;

    quat operator*(quat q) const;

    quat inverted() const;
    void invert();

    quat normalized() const;
    void normalize();
    
    quat rotatedByXAngle(float angle) const;
    quat rotatedByYAngle(float angle) const;
    quat rotatedByZAngle(float angle) const;
    
    void rotateByXAngle(float angle);
    void rotateByYAngle(float angle);
    void rotateByZAngle(float angle);

    void toAxisAngle(vec3* outAxis, float* outAngle) const;
    void toEuler(float* yaw, float* pitch, float* roll) const;
  };

}
