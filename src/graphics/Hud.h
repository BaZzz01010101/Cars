#pragma once
#include "DebugGraphs.h"
#include "Config.h"
#include "MatchStats.h"
#include "Scene.h"

namespace game
{

  struct Hud
  {
    enum CrossHairIndex : int
    {
      Camera = 0,
      Cannon,
      Gun,
      CROSS_HAIR_COUNT
    };

    static constexpr float CROSSHAIR_MOVEMENT_SHARPNESS = 0.3f;
    static constexpr bool IS_VERTICAL_HEALTH_BAR = true;
    const Config& config {};
    const Config::Graphics::Screen& screenConfig {};
    const Config::Graphics::Hud& hudConfig {};
    const MatchStats& matchStats {};

    Font font {};
    Texture crosshairsTexture {};
    mutable DebugGraphs debugGraphs {};
    bool crosshairsTextureLoaded = false;
    bool drawDebugInfo = false;
    bool showNetworkIssuesWarning = false;

    mutable vec2 crossHairPositions[CROSS_HAIR_COUNT] = {
      vec2::zero,
      vec2::zero,
      vec2::zero
    };

    mutable Color lastColor = WHITE;
    mutable float lastPosX = 0;
    mutable float lastPosY = 0;
    mutable float lastFontSize = 10;

    Hud(const Config& config, const MatchStats& matchStats);
    ~Hud();
    Hud(Hud&) = delete;
    Hud(Hud&&) = delete;
    Hud& operator=(Hud&) = delete;
    Hud& operator=(Hud&&) = delete;

    void init();

    void print(const char* text) const;
    void print(const char* text, Color color) const;
    void print(const char* text, Color color, float posX) const;
    void print(const char* text, Color color, float posX, float posY) const;
    void print(const char* text, Color color, float posX, float posY, float fontSize) const;

    void printIf(bool condition, const char* text) const;
    void printIf(bool condition, const char* text, Color color) const;
    void printIf(bool condition, const char* text, Color color, float posX) const;
    void printIf(bool condition, const char* text, Color color, float posX, float posY) const;
    void printIf(bool condition, const char* text, Color color, float posX, float posY, float fontSize) const;

    void print(const char* title, vec3 v) const;
    void print(const char* title, vec3 v, Color color) const;
    void print(const char* title, vec3 v, Color color, float posX) const;
    void print(const char* title, vec3 v, Color color, float posX, float posY) const;
    void print(const char* title, vec3 v, Color color, float posX, float posY, float fontSize) const;

    void print(const char* title, float f) const;
    void print(const char* title, float f, Color color) const;
    void print(const char* title, float f, Color color, float posX) const;
    void print(const char* title, float f, Color color, float posX, float posY) const;
    void print(const char* title, float f, Color color, float posX, float posY, float fontSize) const;

    void update();
    void draw(const CustomCamera& camera, const Scene& scene, float lerpFactor) const;
    void drawCrossHairs(const CustomCamera& camera, const Scene& scene, float lerpFactor) const;
    void drawTurretCrossHair(const CustomCamera& camera, const Scene& scene, const Turret& turret, CrossHairIndex crosshairIndex, float size, Color color, float lerpFactor) const;
    void drawCrossHair(vec2 position, CrossHairIndex crosshairIndex, float size, Color color) const;
    void drawCountdown(const Scene& scene) const;
    void drawLocalPlayerHealth(const Scene& scene) const;
    void drawMatchStats(const Scene& scene) const;
    void drawMatchTimer(const Scene& scene) const;
    void drawNetworkIssuesWarning(const Scene& scene) const;
    void drawDebug(const Scene& scene) const;
  };

}