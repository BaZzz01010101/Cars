#include "core.h"
#include "Args.h"
#include "ClientApp.h"
#include "ServerApp.h"

using namespace game;

int main(int argc, char* argv[])
{
  Args args(argc, argv);

  const Config& config = Config::DEFAULT;

  if (args.mode == Args::Test)
  {
    ServerApp* server = new ServerApp(config, args.serverConfig);
    ClientApp* client = new ClientApp(config, args.serverConfig);
    bool renderServerPlayers = true;

    server->initialize();
    client->initialize();

    client->renderer.inject([&]() {
      if (renderServerPlayers)
      {
        bool dw = client->renderer.drawWires;
        client->renderer.drawWires = true;

        for (int i = 0; i < server->scene.cars.capacity(); i++)
          if (server->scene.cars.isAlive(i))
          {
            const Car& car = server->scene.cars[i];
            client->renderer.drawCar(car, 1);
          }

        client->renderer.drawWires = dw;
      }
    });

    while (client->pulse() && server->pulse())
    {
      if (IsKeyPressed(KEY_Y))
        renderServerPlayers = !renderServerPlayers;
    }

    client->shutdown();
    server->shutdown();

    delete client;
    delete server;
  }
  if (args.mode == Args::Server)
  {
    ServerApp* app = new ServerApp(config, args.serverConfig);

    app->initialize();
    while (app->pulse());
    app->shutdown();

    delete app;
  }
  if (args.mode == Args::Client)
  {
    ClientApp* app = new ClientApp(config, args.serverConfig);

    app->initialize();
    while (app->pulse());
    app->shutdown();

    delete app;
  }
  else
  {
    printf("Unexpected mode in args\n");
  }

  return 0;
}
