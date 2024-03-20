#pragma once

namespace game
{

  class CGEditor
  {
  public:
    void update(float dt);
    void draw(vec3 position, quat rotation, float scale = 1) const;

  private:
    static constexpr int COUNT = 10;
    int index = 0;

    std::pair<vec3, float> spheres[COUNT] = {
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
      { { 0, 5, 0 }, 1 },
    };
  };

}

