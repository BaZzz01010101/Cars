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

		inline static const int HEIGHT_MAP_SIZE = 101;
		inline static const float TERRAIN_SIZE = 100.0f;
		inline static const float TERRAIN_HEIGHT = 10.0f;
		inline static const float CELL_SIZE = TERRAIN_SIZE / (HEIGHT_MAP_SIZE - 1);

		Terrain() = default;
		Terrain(Terrain&) = delete;
		~Terrain();
		Terrain& operator=(Terrain&) = delete;

		void unloadResources();

		float getHeight(float x, float y) const;
		float getHeight(float worldX, float worldY, vec3* normal) const;
		void generate(const char* texturePath, Mode mode);
		bool trace(vec3 start, vec3 end, vec3* hit, vec3* normal) const;
		bool intersectRayTriangle(const vec3 origin, const vec3 direction, const vec3 v0, const vec3 v1, const vec3 v2, vec3* collision, vec3* normal) const;
		void getTriangle(float worldX, float worldY, vec3* v1, vec3* v2, vec3* v3) const;
		void getTrianglePair(int x, int y, vec3* v00, vec3* v01, vec3* v10, vec3* v11) const;
		void draw(bool drawWires);

		// TODO: Rename collision to hitPosition
		// TODO: Rename direction to directionNormalized
		bool traceRay(vec3 origin, vec3 direction, float distance, vec3* collision, vec3* normal) const;

	private:
		struct Cell
		{
			int x;
			int y;
		};

		Mode mode = Mode::Normal;
		int heightMapWidth{};
		int heightMapHeight{};
		vec3 terrainDimensions{};
		std::vector<uint8_t> heightMap;
		std::vector<float> heightMap2;
		Mesh mesh{};
		Model model{};
		Texture texture{};
		bool modelLoaded = false;
		bool textureLoaded = false;

		float calcHeight(float x, float y, Mode mode) const;
		std::vector<Cell> traceGrid2D(vec2 origin, vec2 direction, float distance) const;
	};

}