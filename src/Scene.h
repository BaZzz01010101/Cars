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
    bool paused = false;
    bool drawWires = false;
    bool slowMotion = false;
    bool gunFiring = false;
    bool cannonFiring = false;
    CustomCamera camera;
    vec3 gunRayHit = vec3::zero;
    vec3 cannonRayHit = vec3::zero;

    Scene(const Config& config);
    ~Scene();
    Scene(Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&) = delete;
    Scene& operator=(Scene&&) = delete;

    void init();
    void update(float dt);
    void draw();
    void regenerateTerrain(Terrain::Mode mode);
    void reset(vec3 playerPosition, quat playerRotation);
    const Car& getPlayer() const { return cars.get(playerIndex); }
    Car& getPlayer() { return cars.get(playerIndex); }

  private:
    const Config& config {};
    Terrain terrain;
    int playerIndex = 0;
    Pool<Car, 1> cars {};
    Pool<Projectile, 1000> projectiles {};
    Pool<ExplosionParticle, 10000> explosionParticles {};

    Model carModel {};
    Model wheelModel {};
    Model gunModel {};
    Model cannonModel {};
    Model tree1Model {};
    Model tree2Model {};
    Model rockModel {};

    Texture terrainTexture {};
    Texture tree1Texture {};
    Texture tree2Texture {};
    Texture rockTexture {};

    bool carModelLoaded = false;
    bool wheelModelLoaded = false;
    bool gunModelLoaded = false;
    bool cannonModelLoaded = false;
    bool tree1ModelLoaded = false;
    bool tree2ModelLoaded = false;
    bool rockModelLoaded = false;

    bool terrainTextureLoaded = false;
    bool tree1TextureLoaded = false;
    bool tree2TextureLoaded = false;
    bool rockTextureLoaded = false;

    float timeToNextGunFire = 0;
    float timeToNextCannonFire = 0;

    void updateGameObjects(float dt);
    void updateFiring(float dt);
    void updatePlayerControl(float dt);
    void createExplosion(const Config::Graphics::ExplosionParticles& config, vec3 position);
    void unloadResources();
  };

}