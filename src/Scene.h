#pragma once
#include "Terrain.h"
#include "Car.h"
#include "Config.h"
#include <CustomCamera.h>

namespace game
{

  class Scene
  {
  public:
    CustomCamera camera{};
    Terrain terrain{};
    Car player{};

    Scene() = default;
    Scene(Scene&) = delete;
    ~Scene();
    Scene& operator=(Scene&) = delete;

    void init(const Config& config);
    void update(float dt);
    void draw();
    void regenerateTerrain(const char* texturePath, Terrain::Mode mode);
    inline void setPaused(bool paused) { this->paused = paused; }
    void toggleDrawWires();
    void toggleSlowMotion();
    void reset(vec3 playerPosition, quat playerRotation);

  private:
    bool paused{};
    bool drawWires{};
    bool slowMotion{};

    Model carModel{};
    Model wheelModel{};
    Model turretModel{};

    bool carModelLoaded{};
    bool wheelModelLoaded{};
    bool turretModelLoaded{};
  };

}