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

    float sensitivityModifier = mode == Mode::Zoom ? 0.0001f : 0.001f;

    expectedYaw = normalizeAngle(expectedYaw - mouseDelta.x * cameraConfig.horzSensitivity * sensitivityModifier);
    expectedPitch = normalizeAngle(expectedPitch - mouseDelta.y * cameraConfig.vertSensitivity * sensitivityModifier * (1 - 2 * invertY));
    expectedPitch = std::clamp(expectedPitch, cameraConfig.minPitch, cameraConfig.maxPitch);

    yaw = moveAngleToRelative(yaw, expectedYaw, cameraConfig.rotationSharpness * dt);
    pitch = moveAngleToRelative(pitch, expectedPitch, cameraConfig.rotationSharpness * dt);

    quat rotation = quat::fromXAngle(pitch).rotatedByYAngle(yaw);

    if(mode == Mode::Normal)
    {
      vec3 expectedFocusPosition = playerPosition + vec3 { 0, cameraConfig.focusElevation, 0 };
      focusPosition = moveToRelative(focusPosition, expectedFocusPosition, cameraConfig.pursuitSharpness * dt);
      vec3 focusToCameraDir = vec3::forward.rotatedBy(rotation);
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
      camera.fovy = 90;
    }
    else if (mode == Mode::Zoom)
    {
      focusPosition = playerPosition + vec3 { 0, cameraConfig.focusElevation, 0 };
      vec3 focusToCameraDir = vec3::forward.rotatedBy(rotation);
      distanceFromFocus = 0.01f;
      position = focusPosition + focusToCameraDir * distanceFromFocus;
      direction = -focusToCameraDir;
      camera.fovy = 90 / config.graphics.camera.zoomFactor;
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
