#pragma once
#include "DebugGraphs.h"
#include "Config.h"
#include "Scene.h"

namespace game
{

  struct Hud
  {
    const Config& config {};

    Font font {};
    Texture crosshairsTexture {};
    mutable DebugGraphs debugGraphs {};
    bool crosshairsTextureLoaded = false;
    bool paused = false;

    // TODO: Make enum for cross hair indexes
    mutable vec2 crossHairPositions[3] = {
      vec2::zero,
      vec2::zero,
      vec2::zero
    };

    mutable Color lastColor = WHITE;
    mutable int lastPosX = 0;
    mutable int lastPosY = 0;
    mutable int lastFontSize = 10;

    Hud(const Config& config);
    ~Hud();
    Hud(Hud&) = delete;
    Hud(Hud&&) = delete;
    Hud& operator=(Hud&) = delete;
    Hud& operator=(Hud&&) = delete;

    void init();

    void print(const char* text) const;
    void print(const char* text, Color color) const;
    void print(const char* text, Color color, int posX) const;
    void print(const char* text, Color color, int posX, int posY) const;
    void print(const char* text, Color color, int posX, int posY, int fontSize) const;

    void printIf(bool condition, const char* text) const;
    void printIf(bool condition, const char* text, Color color) const;
    void printIf(bool condition, const char* text, Color color, int posX) const;
    void printIf(bool condition, const char* text, Color color, int posX, int posY) const;
    void printIf(bool condition, const char* text, Color color, int posX, int posY, int fontSize) const;

    void print(const char* title, vec3 v) const;
    void print(const char* title, vec3 v, Color color) const;
    void print(const char* title, vec3 v, Color color, int posX) const;
    void print(const char* title, vec3 v, Color color, int posX, int posY) const;
    void print(const char* title, vec3 v, Color color, int posX, int posY, int fontSize) const;

    void print(const char* title, float f) const;
    void print(const char* title, float f, Color color) const;
    void print(const char* title, float f, Color color, int posX) const;
    void print(const char* title, float f, Color color, int posX, int posY) const;
    void print(const char* title, float f, Color color, int posX, int posY, int fontSize) const;

    void update();
    void draw(const CustomCamera& camera, const Scene& scene) const;
    void drawCrossHairs(const CustomCamera& camera, const Scene& scene) const;
    void drawTurretCrossHair(const CustomCamera& camera, const Scene& scene, const Turret& turret, int textureIndex, float srcSize, float dstSize, Color color) const;
    void drawCrossHair(vec2 position, int textureIndex, float srcSize, float dstSize, Color color) const;
    void drawDebug(const Scene& scene) const;
  };

}