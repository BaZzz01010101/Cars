#pragma once
#include "DebugGraphs.h"
#include "Config.h"
#include <Scene.h>

namespace game
{

  class Hud
  {
  public:
    void init(const Config& config);
    inline void setPaused(bool paused) { this->paused = paused; }

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

  private:
    Config::Graphics::Hud hudConfig{};
    Font font;
    Color lastColor{};
    int lastPosX{};
    int lastPosY{};
    int lastFontSize{};
    bool paused{};
    DebugGraphs debugGraphs{};

    void drawDebug(const Scene& scene);
  };

}