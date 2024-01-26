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
		inline static const int HEIGHT_MAP_SIZE = 31;
		inline static const float TERRAIN_SIZE = 300.0f;
		inline static const float TERRAIN_HEIGHT = 40.0f;

		Terrain();
		Terrain(Terrain&) = delete;
		~Terrain();
		Terrain& operator=(Terrain&) = delete;

		void unloadResources();

		float getHeight(float x, float y) const;
		float getHeight2(float x, float y) const;
		float getHeight2(float worldX, float worldY, vec3* normal) const;
		void generate(const char* texturePath, Mode mode);
		void generate2(const char* texturePath, Mode mode);
		bool trace(vec3 start, vec3 end, vec3* hit, vec3* normal) const;
		void getTriangle(float worldX, float worldY, vec3* v1, vec3* v2, vec3* v3) const;
		bool collidePoint(vec3 position, vec3* collision, float* penetration) const;
		void draw(bool drawWires);

	private:
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
	};

}