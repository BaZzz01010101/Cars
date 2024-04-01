#pragma once
#include "box2.h"

namespace game
{

  struct GridWalker
  {
    typedef std::function<bool(int gridX, int gridY)> Callback;

    box2 spaceBounds {};
    vec2 spaceOrigin {};
    int gridWidth {};
    int gridHeight {};
    float cellSize {};

    GridWalker(const box2&& spaceBounds, float cellSize);

    bool walkByLine(vec3 origin, vec3 directionNormalized, float distance, Callback callback);
  };

}