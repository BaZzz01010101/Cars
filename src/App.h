#pragma once
#include "Scene.h"
#include "Hud.h"

namespace game
{

  class App
  {
  public:
    Config config{};
    Scene scene{};
    Hud hud{};

    void initialize();
    void run();
    void shutdown();
    void update(float dt);
    void draw();

    void togglePaused();

  private:
    bool paused{};

    void drawDebug();
    void updateShortcuts();
  };

}