#pragma once

namespace game
{

  struct Renderable
  {
    const Model& model {};

    Renderable(const Model& model);

    void draw(Matrix transform, bool drawWires);
  };

}