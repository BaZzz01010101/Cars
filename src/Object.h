#pragma once

namespace game
{

  class Object
  {
  public:
    vec3 position{};
    quat rotation = quat::identity;

    virtual ~Object() = default;

    vec3 forward() const;
    vec3 left() const;
    vec3 up() const;

    virtual void update(float dt) = 0;
  };

}