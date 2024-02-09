#pragma once

namespace game
{

  class Positionable
  {
  public:
    vec3 position{};
    quat rotation = quat::identity;

    vec3 forward() const;
    vec3 left() const;
    vec3 up() const;
  };

}