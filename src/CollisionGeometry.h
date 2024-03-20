#pragma once

namespace game
{

  class CollisionGeometry
  {
  public:
    void add(vec3 position, float radius);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    std::pair<vec3, vec3> getBounds() const;
    void drawDebug() const;

  private:
    struct Sphere
    {
      vec3 position {};
      float radius {};

      bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    };

    static constexpr int STATIC_COUNT = 5;
    std::array<Sphere, STATIC_COUNT> spheres { };
    std::vector<Sphere> spheres1;
  };

}