#include "pch.h"
#include "Turret.h"
#include "Helpers.h"

namespace game
{
  Turret::Turret(const Config::Physics::Turret& config, const Model& model, const Terrain& terrain, vec3 connectionPoint, float scale) :
    config(config),
    Renderable(model),
    terrain(terrain),
    connectionPoint(connectionPoint),
    scale(scale)
  {
  }

  void Turret::reset()
  {
    position = vec3::zero;
    rotation = quat::identity;
    yaw = 0;
    pitch = 0;
  }

  vec3 Turret::barrelPosition() const
  {
    return position + config.barrelPosition.rotatedBy(rotation);
  }

  void Turret::update(float dt, const Object& parent)
  {
    //rotation = parent.rotation;// .rotatedByXAngle(pitch).rotatedByYAngle(yaw);

    float targetYaw, targetPitch;
    target.rotatedBy(parent.rotation.inverted()).yawPitch(&targetYaw, &targetPitch);

    yaw = moveTo(yaw, targetYaw, config.rotationSpeed * dt);
    pitch = moveTo(pitch, targetPitch, config.rotationSpeed * dt);

    yaw = clamp(yaw, config.minYaw, config.maxYaw);
    pitch = clamp(pitch, config.minPitch, config.maxPitch);

    vec3 globalConnectionPoint = connectionPoint.rotatedBy(parent.rotation);
    position = parent.position + globalConnectionPoint;
    rotation = parent.rotation * quat::identity.rotatedByXAngle(pitch).rotatedByYAngle(yaw);
    rotation.normalize();

    isRayHit = terrain.traceRay(barrelPosition(), forward(), -1, &rayHitPosition, nullptr);
  }

  void Turret::draw(bool drawWires)
  {
    Matrix transform = MatrixMultiply(MatrixMultiply(QuaternionToMatrix(rotation), MatrixScale(scale, scale, scale)), MatrixTranslate(position.x, position.y, position.z));
    Renderable::draw(transform, drawWires);
    drawDebug();
  }

  void Turret::drawDebug()
  {}

}
