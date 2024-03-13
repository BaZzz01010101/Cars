#pragma once
#include "Scene.h"
#include "Hud.h"

namespace game
{

  class App
  {
  public:
    App();

    void initialize();
    void run();
    void shutdown();

  private:
    Config config {};
    Scene scene;
    Hud hud;

    bool paused = false;

    void togglePaused();
    void update(float dt);
    void updateShortcuts();
    void draw();
    void drawDebug();
  };

}