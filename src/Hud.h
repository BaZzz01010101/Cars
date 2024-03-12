#pragma once
#include "DebugGraphs.h"
#include "Config.h"
#include "Scene.h"

namespace game
{

  class Hud
  {
  public:
    bool paused{};

    Hud(const Config& config);
    ~Hud();
    Hud(Hud&) = delete;
    Hud(Hud&&) = delete;
    Hud& operator=(Hud&) = delete;
    Hud& operator=(Hud&&) = delete;

    void init();

    void print(const char* text);
    void print(const char* text, Color color);
    void print(const char* text, Color color, int posX);
    void print(const char* text, Color color, int posX, int posY);
    void print(const char* text, Color color, int posX, int posY, int fontSize);

    void printIf(bool condition, const char* text);
    void printIf(bool condition, const char* text, Color color);
    void printIf(bool condition, const char* text, Color color, int posX);
    void printIf(bool condition, const char* text, Color color, int posX, int posY);
    void printIf(bool condition, const char* text, Color color, int posX, int posY, int fontSize);

    void print(const char* title, vec3 v);
    void print(const char* title, vec3 v, Color color);
    void print(const char* title, vec3 v, Color color, int posX);
    void print(const char* title, vec3 v, Color color, int posX, int posY);
    void print(const char* title, vec3 v, Color color, int posX, int posY, int fontSize);

    void print(const char* title, float f);
    void print(const char* title, float f, Color color);
    void print(const char* title, float f, Color color, int posX);
    void print(const char* title, float f, Color color, int posX, int posY);
    void print(const char* title, float f, Color color, int posX, int posY, int fontSize);

    void update();
    void draw(const Scene& scene);
    void drawCrossHairs(const Scene& scene);

    void drawTurretCrossHair(const CustomCamera& camera, const Turret& turret, int textureIndex, float srcSize, float dstSize, Color color);

  private:
    const Config& config{};
    Font font{};
    Texture crosshairsTexture{};
    bool crosshairsTextureLoaded{};
    Color lastColor{};
    int lastPosX{};
    int lastPosY{};
    int lastFontSize{};
    DebugGraphs debugGraphs{};

    void drawCrossHair(vec2 position, int textureIndex, float srcSize, float dstSize, Color color);

    void drawDebug(const Scene& scene);
  };

}