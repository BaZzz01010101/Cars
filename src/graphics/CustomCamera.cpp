#include "core.h"
#include "CustomCamera.h"
#include "Helpers.h"

namespace game
{
  CustomCamera::CustomCamera(const Config& config) :
    config(config)
  {
    reset(vec3::zero);
  }

  void CustomCamera::update(float dt, const Terrain& terrain, vec3 playerPosition)
  {
    Vector2 mouseDelta = GetMouseDelta();
    static const Config::Graphics::Camera& cameraConfig = config.graphics.camera;

    expectedYaw = normalizeAngle(expectedYaw - mouseDelta.x * cameraConfig.horzSensitivity * 0.001f);
    expectedPitch = normalizeAngle(expectedPitch - mouseDelta.y * cameraConfig.vertSensitivity * 0.001f * (1 - 2 * invertY));
    expectedPitch = std::clamp(expectedPitch, cameraConfig.minPitch, cameraConfig.maxPitch);

    yaw = moveAngleToRelative(yaw, expectedYaw, cameraConfig.rotationSharpness * dt);
    pitch = moveAngleToRelative(pitch, expectedPitch, cameraConfig.rotationSharpness * dt);

    quat rotation = quat::fromXAngle(pitch).rotatedByYAngle(yaw);

    vec3 expectedFocusPosition = playerPosition + vec3 { 0, cameraConfig.focusElevation, 0 };
    focusPosition = moveToRelative(focusPosition, expectedFocusPosition, cameraConfig.pursuitSharpness * dt);
    vec3 focusToCameraDir = vec3::forward.rotatedBy(rotation);
    vec3 expectedCameraPosition = focusPosition + focusToCameraDir * cameraConfig.maxDistanceFromFocus;
    float hitDistance;

    if (terrain.traceRay(focusPosition, focusToCameraDir, cameraConfig.maxDistanceFromFocus, nullptr, nullptr, &hitDistance))
      distanceFromFocus = moveTo(distanceFromFocus, hitDistance, cameraConfig.collisionSharpness * dt);
    else
      distanceFromFocus = moveTo(distanceFromFocus, cameraConfig.maxDistanceFromFocus, cameraConfig.pursuitSharpness * dt);
    
    position = focusPosition + focusToCameraDir * distanceFromFocus;
    direction = -focusToCameraDir;

    if (terrain.traceRay(position + vec3::up, -vec3::up, 2.0f, nullptr, nullptr, &hitDistance))
    {
      position.y += 2.0f - hitDistance;
      direction = (focusPosition - position).normalized();
    }

    camera.position = position;
    camera.target = focusPosition;
    camera.up = vec3::up;

    HideCursor();
    SetMousePosition(config.graphics.screen.width / 2, config.graphics.screen.height / 2);
  }

  void CustomCamera::reset(vec3 playerPosition)
  {
    focusPosition = playerPosition + vec3::up * Terrain::TERRAIN_HEIGHT * 2;
    position = focusPosition + vec3::up;
    direction = -vec3::up;
    yaw = 0;
    pitch = -PI / 2;
    expectedYaw = yaw;
    expectedPitch = pitch;

    camera.position = position;
    camera.target = playerPosition;
    camera.up = vec3::up;
    camera.fovy = 90;
    camera.projection = CAMERA_PERSPECTIVE;
  }

}
