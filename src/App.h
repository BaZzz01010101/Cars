#pragma once
#include <Scene.h>
#include <Hud.h>

namespace game
{

  class App
  {
  public:
    static constexpr int SCREEN_WIDTH = 1280;
    static constexpr int SCREEN_HEIGHT = 720;
    static constexpr const char* TITLE = "Mad Max";

    Config config;
    Scene scene;
    Hud hud;

    void initialize();
    void run();
    void shutdown();
    void update(float dt);
    void draw();

  private:
    void drawDebug();
    void updateShortcuts();
  };

}