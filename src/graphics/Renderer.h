#pragma once

#include "Scene.h"
#include "Hud.h"

namespace game
{

  struct Renderer
  {
    const Config& config;
    const CustomCamera& camera;
    const Scene& scene;
    const Hud& hud;

    Material wiresMaterial {};
    Material destroyedCarMaterial {};
    Material destroyedCarMaterialTransparent {};

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
    std::function<void()> injectionCallback {};

    Renderer(const Config& config, const CustomCamera& camera, const Scene& scene, const Hud& hud);
    Renderer(Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void init();
    void shutdown();
    void inject(std::function<void()> callback);
    void draw(float lerpFactor);
    void drawDebug(float lerpFactor);
    void drawCar(const Car& car, float lerpFactor);
    void drawHealthBar(vec3 position, int health);
    void drawDynamicObject(const DynamicObject& dynamicObject, const Model& model, float lerpFactor, std::optional<Material> overriddenMaterial = std::nullopt);
    void drawModel(const Model& model, const Matrix& transform, std::optional<Material> overriddenMaterial = std::nullopt);
    void drawProjectile(const Projectile& projectile, float lerpFactor);
    void drawExplosionParticle(const ExplosionParticle& explosionParticle, float lerpFactor);
    void drawTerrain();
    void drawTerrainObject(const TerrainObject& terrainObject);

    void drawTerrainDebug();
    void drawCarDebug(const Car& car, float lerpFactor);
    void drawWheelDebug(const Wheel& wheel, float lerpFactor);

    void loadResources();
    void unloadResources();
    void updateTerrainModel();
    void visualizeTerrainRayTracing(vec3 origin, vec3 directionNormalized, float distance) const;
  };

}
