#pragma once
class Renderable
{
public:
  Renderable(const Model& model);

  void draw(Matrix transform, bool drawWires);

protected:
  const Model& model {};
};
