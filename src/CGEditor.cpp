#include "pch.h"
#include "CGEditor.h"

namespace game
{
  void CGEditor::update(float dt)
  {
    if (IsKeyDown(KEY_KP_0))
      index = 0;
    else if (IsKeyDown(KEY_KP_1))
      index = 1;
    else if (IsKeyDown(KEY_KP_2))
      index = 2;
    else if (IsKeyDown(KEY_KP_3))
      index = 3;
    else if (IsKeyDown(KEY_KP_4))
      index = 4;
    else if (IsKeyDown(KEY_KP_5))
      index = 5;
    else if (IsKeyDown(KEY_KP_6))
      index = 6;
    else if (IsKeyDown(KEY_KP_7))
      index = 7;
    else if (IsKeyDown(KEY_KP_8))
      index = 8;
    else if (IsKeyDown(KEY_KP_9))
      index = 9;

    float& axis = IsKeyDown(KEY_LEFT_CONTROL) ? spheres[index].first.x :
      IsKeyDown(KEY_LEFT_SHIFT) ? spheres[index].first.y :
      IsKeyDown(KEY_LEFT_ALT) ? spheres[index].first.z :
      spheres[index].second;

    if (IsKeyDown(KEY_KP_ADD))
      axis += 0.5f * dt;
    else if (IsKeyDown(KEY_KP_SUBTRACT))
      axis -= 0.5f * dt;

  }
  void CGEditor::draw(vec3 position, quat rotation, float scale) const
  {
    for (int i = 0; i < COUNT; i++)
    {
      auto& [center, radius] = spheres[i];
      vec3 pos = position + scale * center.rotatedBy(rotation);

      DrawSphere(pos, scale * radius, i == index ? YELLOW : RED);
    }
  }
}