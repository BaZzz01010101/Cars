#pragma once
#include "Config.h"
#include "Scene.h"
#include "PlayerState.h"
#include "Queue.hpp"
#include "Connection.h"

namespace game
{

  struct ServerApp
  {
    static volatile bool exit;

    Config config {};
    Scene scene;
    Connection connection {};
    high_resolution_clock clock {};

    ServerApp();

    void initialize();
    void run();
    void shutdown();
  };

}