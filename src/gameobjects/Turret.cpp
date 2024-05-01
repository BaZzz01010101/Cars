#include "core.h"
#include "Turret.h"
#include "Helpers.h"
#include "Scene.h"

namespace game
{
  Turret::Turret(const Config::Physics::Turret& config, const Scene& scene, vec3 connectionPoint) :
    config(config),
    scene(scene),
    connectionPoint(connectionPoint)
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

  vec3 Turret::barrelPosition(float lerpFactor) const
  {
    vec3 position = vec3::lerp(this->lastPosition, this->position, lerpFactor);
    quat rotation = quat::slerp(this->lastRotation, this->rotation, lerpFactor);

    return position + config.barrelPosition.rotatedBy(rotation);
  }

  void Turret::syncState(TurretState turretState, float syncFactor, const Object& parent)
  {
    yaw = lerp(yaw, turretState.yaw, syncFactor);
    pitch = lerp(pitch, turretState.pitch, syncFactor);
    vec3 globalConnectionPoint = connectionPoint.rotatedBy(parent.rotation);
    position = parent.position + globalConnectionPoint;
    rotation = parent.rotation * quat::identity.rotatedByXAngle(pitch).rotatedByYAngle(yaw);
    rotation.normalize();
  }

  TurretState Turret::getState() const
  {
    return TurretState {
      .yaw = yaw,
      .pitch = pitch,
    };
  }

  void Turret::update(float dt, const Object& parent)
  {
    lastPosition = position;
    lastRotation = rotation;

    vec3 connectionPointRotated = connectionPoint.rotatedBy(parent.rotation);
    position = parent.position + connectionPointRotated;
    rotation = parent.rotation * quat::fromXAngle(pitch).rotatedByYAngle(yaw);
    vec3 pos = position + vec3 { 0, config.barrelPosition.y, 0 }.rotatedBy(parent.rotation);

    float expectedYaw, expectedPitch;
    vec3 barrelToExpectedTarget = expectedTarget - pos;
    // TODO: Possible excess rotations, consider optimization
    barrelToExpectedTarget.rotatedBy(parent.rotation.inverted()).yawPitch(&expectedYaw, &expectedPitch);

    // TODO: This code does not support 360° rotations. Need additional checks
    // for this case to rotate turret in the right direction by shortest path
    yaw = moveTo(yaw, expectedYaw, config.rotationSpeed * dt);
    pitch = moveTo(pitch, expectedPitch, config.rotationSpeed * dt);

    yaw = clamp(yaw, config.minYaw, config.maxYaw);
    pitch = clamp(pitch, config.minPitch, config.maxPitch);

    rotation = parent.rotation * quat::fromXAngle(pitch).rotatedByYAngle(yaw);
    rotation.normalize();

    // TODO: Move tracing to Hud::drawTurretCrossHair to avoid excess computations on server and for remote players
    float distanceToTarget = barrelToExpectedTarget.length();

    if (!scene.traceRay(pos, forward(), distanceToTarget, scene.localPlayerIndex, &target, nullptr, nullptr, nullptr))
      target = pos + forward() * distanceToTarget;
  }

}
