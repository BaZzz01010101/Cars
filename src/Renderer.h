#pragma once

#include "Scene.h"
#include "Hud.h"

namespace game
{

  struct Renderer
  {
    const Config& config;
    const Scene& scene;
    const Hud& hud;

    Material wiresMaterial {};

    Model terrainModel {};
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

    bool drawWires = false;

    Renderer(const Config& config, const Scene& scene, const Hud& hud);
    Renderer(Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void init();
    void shutdown();
    void draw();
    void drawCar(const Car& car);
    void drawWheel(const Wheel& wheel);
    void drawTurret(const Turret& turret);
    void drawModel(const Model& model, const Matrix& transform);
    void drawProjectile(const Projectile& projectile);
    void drawExplosionParticle(const ExplosionParticle& explosionParticle);
    void drawTerrain();
    void drawTerrainObject(const TerrainObject& terrainObject);

    void drawTerrainDebug();
    void drawCarDebug(const Car& car);
    void drawWheelDebug(const Wheel& wheel);

    void loadResources();
    void unloadResources();
    void updateTerrainModel();

    void visualizeTerrainRayTracing(vec3 origin, vec3 directionNormalized, float distance) const;
  };

}
