#include "core.h"
#include "Turret.h"
#include "Helpers.h"

namespace game
{
  Turret::Turret(const Config::Physics::Turret& config, const Terrain& terrain, vec3 connectionPoint, float scale) :
    config(config),
    terrain(terrain),
    connectionPoint(connectionPoint),
    scale(scale)
  {}

  void Turret::reset()
  {
    lastPosition = vec3::zero;
    lastRotation = quat::identity;

    position = vec3::zero;
    rotation = quat::identity;
    yaw = 0;
    pitch = 0;
  }

  vec3 Turret::barrelPosition() const
  {
    return position + config.barrelPosition.rotatedBy(rotation);
  }

  void Turret::syncState(float yaw, float pitch, float syncFactor, const Object& parent)
  {
    this->yaw = lerp(this->yaw, yaw, syncFactor);
    this->pitch = lerp(this->pitch, pitch, syncFactor);
    vec3 globalConnectionPoint = connectionPoint.rotatedBy(parent.rotation);
    position = parent.position + globalConnectionPoint;
    rotation = parent.rotation * quat::identity.rotatedByXAngle(pitch).rotatedByYAngle(yaw);
    rotation.normalize();
  }

  void Turret::update(float dt, const Object& parent)
  {
    lastPosition = position;
    lastRotation = rotation;

    float targetYaw, targetPitch;
    vec3 pos = position + vec3 { 0, config.barrelPosition.y, 0 }.rotatedBy(rotation);
    (expectedTarget - pos).rotatedBy(parent.rotation.inverted()).yawPitch(&targetYaw, &targetPitch);

    yaw = moveTo(yaw, targetYaw, config.rotationSpeed * dt);
    pitch = moveTo(pitch, targetPitch, config.rotationSpeed * dt);

    yaw = clamp(yaw, config.minYaw, config.maxYaw);
    pitch = clamp(pitch, config.minPitch, config.maxPitch);

    vec3 globalConnectionPoint = connectionPoint.rotatedBy(parent.rotation);
    position = parent.position + globalConnectionPoint;
    rotation = parent.rotation * quat::identity.rotatedByXAngle(pitch).rotatedByYAngle(yaw);
    rotation.normalize();

    // TODO: Consider better targeting method
    // In current implementation the cross hair sometimes behaves unpreditably
    // when tracing not hit the terrain
    //pos = position + vec3 { 0, config.barrelPosition.y, 0 }.rotatedBy(rotation);
    //if (!terrain.traceRay(pos, forward(), FLT_MAX, &currentTarget, nullptr, nullptr))
      currentTarget = position + 1000 * forward();
  }

}
