#include "pch.h"
#include "CustomCamera.h"
#include "Helpers.h"

namespace game
{
  void CustomCamera::init(const Config::Graphics& config)
  {
    graphicsConfig = config;
    camera.fovy = 90;
    camera.projection = CAMERA_PERSPECTIVE;
  }

  void CustomCamera::update(float dt, const Terrain& terrain, vec3 playerPosition)
  {
    Vector2 mouseDelta = GetMouseDelta();

    yaw -= mouseDelta.x * graphicsConfig.camera.horzSensitivity * 0.001f;
    yaw = normalizeAngle(yaw);

    pitch -= mouseDelta.y * graphicsConfig.camera.vertSensitivity * 0.001f * (1 - 2 * graphicsConfig.camera.invertY);
    pitch = std::clamp(pitch, -graphicsConfig.camera.maxPitch, graphicsConfig.camera.maxPitch);

    quat rotation = quat::fromYAngle(yaw) * quat::fromXAngle(pitch);

    vec3 focusPoint = playerPosition + vec3{ 0, graphicsConfig.camera.focusElevation, 0 };
    position = focusPoint + vec3{ 0, 0, graphicsConfig.camera.minDistance }.rotatedBy(rotation);
    float minYPosition = terrain.getHeight2(position.x, position.z) + 0.5f;
    position.y = std::max(position.y, minYPosition);

    vec3 dPos = focusPoint - position;
    float dy = dPos.y;
    float dxz = sqrt(sqr(dPos.x) + sqr(dPos.z));

    if(dy > 0 && dxz > 0)
      pitch = std::atan2(dy, dxz);

    direction = (focusPoint - position).normalized();

    camera.position = position;
    camera.target = focusPoint;
    camera.up = vec3::up;

    HideCursor();
    SetMousePosition(graphicsConfig.screen.width / 2, graphicsConfig.screen.height / 2);
    UpdateCamera(&camera, CAMERA_CUSTOM);
  }

  void CustomCamera::reset(vec3 playerPosition)
  {
    camera.position = playerPosition + vec3{ -4, 1, 0 };
    camera.target = playerPosition;
    camera.up = { 0, 1, 0 };
  }

}
