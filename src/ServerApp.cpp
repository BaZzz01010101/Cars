#include "pch.h"
#include "ServerApp.h"
#include "Helpers.h"

namespace game
{
  volatile bool ServerApp::exit = false;

  ServerApp::ServerApp() :
    config(Config::DEFAULT),
    scene(config)
  {
  }

  void ServerApp::initialize()
  {
    printf("Server started:\n");
    scene.init();
  }

  void ServerApp::run()
  {
    signal(SIGINT, [](int) { exit = true; });
    nanoseconds dt = milliseconds(int(1000 * config.physics.maxDt));
    nanoseconds maxSleep = milliseconds(0);

    for (int i = 0; i < 100; i++)
    {
      time_point tp = clock.now();
      std::this_thread::sleep_for(milliseconds(0));
      maxSleep = std::max(maxSleep, clock.now() - tp);
    }

    maxSleep *= 2;

    time_point tp0 = clock.now();
    PlayerState playerState;

    while (!exit)
    {
      time_point tp1 = clock.now();
      nanoseconds elapsed = tp1 - tp0;

      if (elapsed < dt - maxSleep)
        std::this_thread::sleep_for(milliseconds(1));
      else if(elapsed >= dt)
      {
        tp0 += dt;

        while(connection.readPlayerState(&playerState))
          scene.syncPlayerState(playerState);

        scene.update(config.physics.maxDt);

        for(int i=0; i< scene.cars.capacity(); i++)
          if(scene.cars.isAlive(i))
          {
            scene.getPlayerState(i, &playerState);
            connection.writePlayerState(playerState);
          }
      }
    }
  }

  void ServerApp::shutdown()
  {
  }

}
