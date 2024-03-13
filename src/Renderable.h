#pragma once
class Renderable
{
public:
  Renderable(const Model& model);

  void draw(Matrix transform, bool drawWires);

private:
  const Model& model {};
};
