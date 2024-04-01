#pragma once
#include "Scene.h"
#include "Hud.h"

namespace game
{

  struct App
  {
    Config config {};
    Scene scene;
    Hud hud;

    bool paused = false;

    App();

    void initialize();
    void run();
    void shutdown();
    void togglePaused();
    void update(float dt);
    void updateShortcuts();
    void draw();
    void drawDebug();
    void updatePlayerControl();
  };

}