#pragma once

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
    static constexpr float TERRAIN_SIZE = 100.0f;
    static constexpr float TERRAIN_SIZE_2 = TERRAIN_SIZE / 2;
    static constexpr float TERRAIN_HEIGHT = 10.0f;
    static constexpr int GRID_SIZE = HEIGHT_MAP_SIZE - 1;
    static constexpr float CELL_SIZE = TERRAIN_SIZE / GRID_SIZE;

    Terrain() = default;
    ~Terrain();
    Terrain(Terrain&) = delete;
    Terrain(Terrain&&) = delete;
    Terrain& operator=(Terrain&) = delete;
    Terrain& operator=(Terrain&&) = delete;

    float getHeight(float worldX, float worldY, vec3* normal = nullptr) const;
    void generate(const char* texturePath, Mode mode);
    bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* normal) const;
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

      bool traceRay(vec3 origin, vec3 directionNormalized, vec3* hitPosition, vec3* normal) const;
    };

    struct TrianglePair
    {
      vec3 v00 = vec3::zero;
      vec3 v10 = vec3::zero;
      vec3 v01 = vec3::zero;
      vec3 v11 = vec3::zero;

      bool traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* normal) const;
    };

    Mode mode = Mode::Normal;
    std::vector<float> heightMap;
    Mesh mesh {};
    Model model {};
    Texture texture {};
    bool modelLoaded = false;
    bool textureLoaded = false;

    void unloadResources();
    float calcHeight(int x, int y, Mode mode) const;
    Triangle getTriangle(float worldX, float worldZ) const;
    TrianglePair getTrianglePair(int x, int y) const;
  };

}