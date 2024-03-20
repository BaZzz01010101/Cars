#include "pch.h"
#include "Renderable.h"

Renderable::Renderable(const Model& model) :
  model(model)
{}

void Renderable::draw(Matrix transform, bool drawWires)
{
  for (int i = 0; i < model.meshCount; i++)
  {
    if (!drawWires)
      DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], transform);

    rlEnableWireMode();
    Material mat = LoadMaterialDefault();
    mat.maps->color = Color { 255, 255, 255, 64 };
    DrawMesh(model.meshes[i], mat, transform);
    rlDisableWireMode();
  }
}
