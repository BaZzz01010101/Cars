#pragma once
#include "Terrain.h"
#include "Car.h"
#include "Config.h"

namespace game
{

  class Scene
  {
  public:
    Camera camera;
    Terrain terrain;
    Car player;

    ~Scene();

    void init(const Config& config);
    void update(float dt);
    void draw();
    void regenerateTerrain(const char * texturePath, Terrain::Mode mode);
    void togglePause();
    void toggleDrawWires();
    void toggleSlowMotion();
    void reset(vec3 playerPosition, quat playerRotation);

  private:
    bool paused;
    bool drawWires;
    bool slowMotion;

    Model carModel{};
    Model wheelModel{};
    Model turretModel{};

    bool carModelLoaded{};
    bool wheelModelLoaded{};
    bool turretModelLoaded{};

    void followPlayer(Camera& camera, Car& player);
  };

}