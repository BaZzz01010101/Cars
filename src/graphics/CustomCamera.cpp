#include "core.h"
#include "CustomCamera.h"
#include "Helpers.h"

namespace game
{
  CustomCamera::CustomCamera(const Config& config) :
    config(config)
  {
    camera.fovy = 90;
    camera.projection = CAMERA_PERSPECTIVE;
  }

  void CustomCamera::update(float dt, const Terrain& terrain, vec3 playerPosition)
  {
    // TODO: Fix potential incorrect calculations of camera center
    // getting mouse delta inside update can lead to incorrect calculations due to the multiple
    // accounting of the same mouse delta in recurcive update calls in App::update because RayLib
    // seems updating input state once per frame
    Vector2 mouseDelta = GetMouseDelta();
    const Config::Graphics::Camera cameraConfig = config.graphics.camera;

    yaw -= mouseDelta.x * cameraConfig.horzSensitivity * 0.001f;
    yaw = normalizeAngle(yaw);

    pitch += mouseDelta.y * cameraConfig.vertSensitivity * 0.001f * (1 - 2 * invertY);
    pitch = std::clamp(pitch, -cameraConfig.maxPitch, cameraConfig.maxPitch);

    quat rotation = quat::fromYAngle(yaw) * quat::fromXAngle(pitch);

    vec3 focusPoint = playerPosition + vec3 { 0, cameraConfig.focusElevation, 0 };
    position = focusPoint + vec3 { 0, 0, cameraConfig.minDistance }.rotatedBy(rotation);
    float minYPosition = terrain.getHeight(position.x, position.z) + 0.5f;
    position.y = std::max(position.y, minYPosition);

    vec3 dPos = focusPoint - position;
    float dy = dPos.y;
    float dxz = sqrt(sqr(dPos.x) + sqr(dPos.z));

    if (dy > 0 && dxz > 0)
      pitch = std::atan2(dy, dxz);

    direction = (focusPoint - position).normalized();

    camera.position = position;
    camera.target = focusPoint;
    camera.up = vec3::up;

    HideCursor();
    SetMousePosition(config.graphics.screen.width / 2, config.graphics.screen.height / 2);
  }

  void CustomCamera::reset(vec3 playerPosition)
  {
    camera.position = playerPosition + vec3 { -4, 1, 0 };
    camera.target = playerPosition;
    camera.up = { 0, 1, 0 };
  }

  vec3 CustomCamera::getTarget() const
  {
    return position + direction * 1000;
  }

}
