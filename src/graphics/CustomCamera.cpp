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
    static const Config::Graphics::Camera& cameraConfig = config.graphics.camera;
    Vector2 mouseDelta {};

    if(IsWindowFocused())
    {
      mouseDelta = GetMouseDelta();
      HideCursor();
      SetMousePosition(config.graphics.screen.width / 2, config.graphics.screen.height / 2);
    }
    else
      ShowCursor();

    float sensitivityModifier = mode == Mode::Zoom ? 0.0001f : 0.001f;

    expectedYaw = normalizeAngle(expectedYaw - mouseDelta.x * cameraConfig.horzSensitivity * sensitivityModifier);
    expectedPitch = normalizeAngle(expectedPitch - mouseDelta.y * cameraConfig.vertSensitivity * sensitivityModifier * (1 - 2 * invertY));
    expectedPitch = std::clamp(expectedPitch, cameraConfig.minPitch, cameraConfig.maxPitch);

    yaw = moveAngleToRelative(yaw, expectedYaw, cameraConfig.rotationSharpness * dt);
    pitch = moveAngleToRelative(pitch, expectedPitch, cameraConfig.rotationSharpness * dt);

    quat rotation = quat::fromXAngle(pitch).rotatedByYAngle(yaw);
    const float ZOOM_SPEED = 1000;

    vec3 focusToCameraDir = vec3::forward.rotatedBy(rotation);
    vec3 expectedFocusPosition = playerPosition + vec3 { 0, cameraConfig.focusElevation, 0 };
    focusPosition = moveToRelative(focusPosition, expectedFocusPosition, cameraConfig.pursuitSharpness * dt);

    float hitDistance;
    constexpr float CAMERA_HIT_OFFSET = 1.0f;

    if (terrain.traceRay(focusPosition, focusToCameraDir, cameraConfig.maxDistanceFromFocus + CAMERA_HIT_OFFSET, nullptr, nullptr, &hitDistance))
    {
      hitDistance = std::max(hitDistance, CAMERA_HIT_OFFSET + 0.01f);
      distanceFromFocus = moveTo(distanceFromFocus, hitDistance - CAMERA_HIT_OFFSET, cameraConfig.collisionSharpness * dt);
    }
    else
      distanceFromFocus = moveTo(distanceFromFocus, cameraConfig.maxDistanceFromFocus, cameraConfig.pursuitSharpness * dt);

    if(mode == Mode::Normal)
      camera.fovy = moveTo(camera.fovy, 90, ZOOM_SPEED * dt);
    else if (mode == Mode::Zoom)
      camera.fovy = moveTo(camera.fovy, 90 / config.graphics.camera.zoomFactor, ZOOM_SPEED * dt);

    position = focusPosition + focusToCameraDir * distanceFromFocus;
    direction = -focusToCameraDir;

    camera.position = position;
    camera.target = focusPosition;
    camera.up = vec3::up;
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
