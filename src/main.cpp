#include "core.h"
#include "ClientApp.h"
#include "ServerApp.h"

using namespace game;

int main(int argc, char* argv[])
{
  const Config& config = Config::DEFAULT;

  if (argc > 1 && !strcmp(argv[1], "-test"))
  {
    ServerApp* server = new ServerApp(config);
    ClientApp* client = new ClientApp(config);
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
  else if (argc > 1 && !strcmp(argv[1], "-server"))
  {
    ServerApp* app = new ServerApp(config);

    app->initialize();
    while (app->pulse());
    app->shutdown();

    delete app;
  }
  else
  {
    ClientApp* app = new ClientApp(config);

    app->initialize();
    while (app->pulse());
    app->shutdown();

    delete app;
  }

  return 0;
}
