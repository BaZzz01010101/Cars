#pragma once
#include "Terrain.h"
#include "Car.h"
#include "Config.h"
#include "CustomCamera.h"
#include "Pool.hpp"
#include "Projectile.h"
#include "ExplosionParticle.h"

namespace game
{
  class Scene
  {
    friend class Hud;
  public:
    bool paused{};
    bool drawWires{};
    bool slowMotion{};
    bool gunFiring{};
    bool cannonFiring{};
    CustomCamera camera;
    vec3 gunRayHit{};
    vec3 cannonRayHit{};

    Scene(const Config& config);
    ~Scene();
    Scene(Scene&) = delete;
    Scene& operator=(Scene&) = delete;

    void init();
    void update(float dt);
    void draw();
    void regenerateTerrain(const char* texturePath, Terrain::Mode mode);
    void reset(vec3 playerPosition, quat playerRotation);
    const Car& getPlayer() const { return cars.get(playerIndex); }
    Car& getPlayer() { return cars.get(playerIndex); }

  private:
    const Config& config{};
    Terrain terrain{};
    int playerIndex{};
    Pool<Car, 1> cars{};
    Pool<Projectile, 1000> projectiles{};
    Pool<ExplosionParticle, 10000> explosionParticles{};

    Model carModel{};
    Model wheelModel{};
    Model gunModel{};
    Model cannonModel{};

    bool carModelLoaded{};
    bool wheelModelLoaded{};
    bool gunModelLoaded{};
    bool cannonModelLoaded{};

    float timeToNextGunFire{};
    float timeToNextCannonFire{};

    void updateGameObjects(float dt);
    void updateFiring(float dt);
    void updatePlayerControl(float dt);
    void createExplosion(const Config::Graphics::ExplosionParticles& config, vec3 position);
    void unloadResources();
  };

}