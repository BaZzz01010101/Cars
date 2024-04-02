#pragma once
#include "Scene.h"
#include "Hud.h"
#include "Config.h"
#include "Renderer.h"

namespace game
{

  struct App
  {
    Config config {};
    CustomCamera camera;
    Scene scene;
    Hud hud;
    Renderer renderer;

    bool paused = false;

    App();

    void initialize();
    void run();
    void shutdown();
    void togglePaused();
    void update(float dt);
    void updateShortcuts();
    void updatePlayerControl();
  };

}