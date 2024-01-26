#pragma once
class Renderable
{
  public:
    void init(const Model& model);
    void draw(const Matrix& transform, bool drawWires);

  private:
    Model model;
};

