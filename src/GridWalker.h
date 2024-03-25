#pragma once
#include "box2.h"

namespace game
{

  class GridWalker
  {
  public:
    typedef std::function<bool(int gridX, int gridY)> Callback;

    GridWalker(const box2&& spaceBounds, float cellSize);

    bool walkByLine(vec3 origin, vec3 directionNormalized, float distance, Callback callback);

  private:
    box2 spaceBounds {};
    vec2 spaceOrigin {};
    int gridWidth {};
    int gridHeight {};
    float cellSize {};
  };

}