#pragma once
class Renderable
{
public:
  virtual ~Renderable() = default;

  void init(const Model& model);
  void draw(bool drawWires);

protected:
  Matrix transform{};

  virtual void updateTransform() = 0;
  virtual void drawDebug() = 0;

private:
  Model model{};
};

