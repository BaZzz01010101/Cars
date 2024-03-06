#include "pch.h"
#include "Renderable.h"

void Renderable::init(const Model& model)
{
  this->model = model;
}

void Renderable::draw(Matrix transform, bool drawWires)
{
  for (int i = 0; i < model.meshCount; i++)
  {
    if(!drawWires)
      DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], transform);

    rlEnableWireMode();
    DrawMesh(model.meshes[i], LoadMaterialDefault(), transform);
    rlDisableWireMode();
  }
}
