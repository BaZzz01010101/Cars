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
  {
  }

  void Turret::reset(const Object& parent)
  {
    yaw = 0;
    pitch = 0;
    updatePositionAndRotation(parent);
    lastPosition = position;
    lastRotation = rotation;
    expectedTarget = position + 100 * forward();
  }

  vec3 Turret::barrelFrontPosition() const
  {
    return position + vec3(0, config.barrelElevation, config.barrelLength).rotatedBy(rotation);
  }

  vec3 Turret::barrelBackPosition() const
  {
    return position + vec3(0, config.barrelElevation, 0).rotatedBy(rotation);
  }

  vec3 Turret::barrelFrontPosition(float lerpFactor) const
  {
    return vec3::lerp(lastPosition, position, lerpFactor) + vec3(0, config.barrelElevation, config.barrelLength).rotatedBy(rotation);
  }

  vec3 Turret::barrelBackPosition(float lerpFactor) const
  {
    return vec3::lerp(lastPosition, position, lerpFactor) + vec3(0, config.barrelElevation, 0).rotatedBy(rotation);
  }

  void Turret::syncState(TurretState turretState, float syncFactor, const Object& parent)
  {
    yaw = moveToRelative(yaw, turretState.yaw, 0.1f * syncFactor);
    pitch = moveToRelative(pitch, turretState.pitch, 0.1f * syncFactor);
    updatePositionAndRotation(parent);
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

    float expectedYaw, expectedPitch;
    vec3 barrelToExpectedTarget = expectedTarget - barrelBackPosition();
    barrelToExpectedTarget.rotatedBy(parent.rotation.inverted()).yawPitch(&expectedYaw, &expectedPitch);

    // TODO: This code does not support 360� rotations. Need additional checks
    // for this case to rotate turret in the right direction by shortest path
    static constexpr float DECELERATION_ANGLE = 5 * DEG2RAD;
    static constexpr float MIN_DECELERATION_FACTOR = 0.01f;
    float yawDecelerationFactor = clamp(fabsf(yaw - expectedYaw) / DECELERATION_ANGLE, MIN_DECELERATION_FACTOR, 1.0f);
    float pitchDecelerationFactor = clamp(fabsf(pitch - expectedPitch) / DECELERATION_ANGLE, MIN_DECELERATION_FACTOR, 1.0f);
    yaw = moveTo(yaw, expectedYaw, config.rotationSpeed * dt * yawDecelerationFactor);
    pitch = moveTo(pitch, expectedPitch, config.rotationSpeed * dt * pitchDecelerationFactor);

    yaw = clamp(yaw, config.minYaw, config.maxYaw);
    pitch = clamp(pitch, config.minPitch, config.maxPitch);

    updatePositionAndRotation(parent);
  }

  void Turret::updateLocked(float dt, const Object& parent)
  {
    lastPosition = position;
    lastRotation = rotation;

    updatePositionAndRotation(parent);
  }

  void Turret::updatePositionAndRotation(const Object& parent)
  {
    vec3 rotatedConnectionPoint = connectionPoint.rotatedBy(parent.rotation);
    position = parent.position + rotatedConnectionPoint;
    rotation = parent.rotation * quat::fromXAngle(pitch).rotatedByYAngle(yaw);
    rotation.normalize();
  }

}
