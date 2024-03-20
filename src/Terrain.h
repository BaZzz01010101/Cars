#pragma once
#include "Pool.hpp"
#include "TerrainObject.h"
#include "CollisionGeometry.h"
#include "Helpers.h"

namespace game
{

  class Terrain
  {
  public:
    enum Mode
    {
      Normal,
      Debug1,
      Debug2
    };

    static constexpr int HEIGHT_MAP_SIZE = 101;
    static constexpr int HEIGHT_MAP_SIZE_2 = HEIGHT_MAP_SIZE / 2;
    static constexpr int HEIGHT_MAP_SIZE_4 = HEIGHT_MAP_SIZE / 4;
    static constexpr float TERRAIN_SIZE = 200.0f;
    static constexpr float TERRAIN_SIZE_2 = TERRAIN_SIZE / 2;
    static constexpr float TERRAIN_HEIGHT = 20.0f;
    static constexpr int GRID_SIZE = HEIGHT_MAP_SIZE - 1;
    static constexpr float CELL_SIZE = TERRAIN_SIZE / GRID_SIZE;
    static constexpr int OBJECT_COUNT = int(TERRAIN_SIZE * TERRAIN_SIZE / 1000);
    static constexpr int CG_GRID_SIZE = std::max(ct_sqrt(OBJECT_COUNT) / 4, 1);

    Terrain(const Texture& terrainTexture, const Model& tree1Model, const Model& tree2Model, const Model& rockModel);
    ~Terrain();
    Terrain(Terrain&) = delete;
    Terrain(Terrain&&) = delete;
    Terrain& operator=(Terrain&) = delete;
    Terrain& operator=(Terrain&&) = delete;

    void init();
    float getHeight(float worldX, float worldY, vec3* normal = nullptr) const;
    void generate(Mode mode);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    void draw(bool drawWires);

  private:
    struct Cell
    {
      int x = 0;
      int y = 0;
    };

    struct Triangle
    {
      vec3 v0 = vec3::zero;
      vec3 v1 = vec3::zero;
      vec3 v2 = vec3::zero;

      bool traceRay(vec3 origin, vec3 directionNormalized, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    };

    struct TrianglePair
    {
      vec3 v00 = vec3::zero;
      vec3 v10 = vec3::zero;
      vec3 v01 = vec3::zero;
      vec3 v11 = vec3::zero;

      bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    };

    const static Matrix transform;

    Mode mode = Mode::Normal;
    std::vector<float> heightMap;
    Pool<TerrainObject, OBJECT_COUNT> objects {};
    Pool<CollisionGeometry, OBJECT_COUNT> objectCollisionGeometries {};
    int cgGrid[CG_GRID_SIZE][CG_GRID_SIZE] {};
    Mesh mesh {};
    Model model {};
    Material wiresMaterial {};
    const Model& tree1Model {};
    const Model& tree2Model {};
    const Model& rockModel {};
    const Texture& terrainTexture {};
    const Texture& tree1Texture {};
    const Texture& tree2Texture {};
    const Texture& rockTexture {};
    bool modelLoaded = false;

    static CollisionGeometry createTree1CollisionGeometry(vec3 position, float angle, float scale);
    static CollisionGeometry createTree2CollisionGeometry(vec3 position, float angle, float scale);
    static CollisionGeometry createRockCollisionGeometry(vec3 position, float angle, float scale);

    void unloadResources();
    void generateObjects();
    bool traceRayWithTerrain(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const;
    float calcHeight(int x, int y, Mode mode) const;
    Triangle getTriangle(float worldX, float worldZ) const;
    TrianglePair getTrianglePair(int x, int y) const;
    void drawDebug() const;
  };

}