#pragma once
#include "Config.h"
#include "Scene.h"
#include "Hud.h"
#include "Renderer.h"
#include "Connection.h"

namespace game
{

  struct App
  {
    Config config {};
    CustomCamera camera;
    Scene scene;
    Hud hud;
    Renderer renderer;
    Connection connection {};

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