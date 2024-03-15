#pragma once
#include "Object.h"
#include "Renderable.h"

namespace game
{

  struct TerrainObject : public Renderable
  {
    TerrainObject(const Model& model, vec3 position, float angle, float scale);
    TerrainObject(TerrainObject&) = delete;
    TerrainObject(TerrainObject&&) = delete;
    TerrainObject& operator=(TerrainObject&) = delete;
    TerrainObject& operator=(TerrainObject&&) = delete;

    void draw(bool drawWires);

    Matrix transform;
  };

}