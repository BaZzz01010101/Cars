#include "core.h"
#include "Args.h"
#include "ClientApp.h"
#include "ServerApp.h"

using namespace game;

void runServer(const Args& args, const Config& config)
{
  ServerApp* app = new ServerApp(config, args.serverConfig);

  app->initialize();
  while (app->pulse());
  app->shutdown();

  delete app;
}

void runClient(const Args& args, const Config& config)
{
  ClientApp* app = new ClientApp(config, args.windowConfig, args.serverConfig);

  app->initialize();
  while (app->pulse());
  app->shutdown();

  delete app;
}

void runBoth(const Args& args, const Config& config)
{
  ServerApp* server = new ServerApp(config, args.serverConfig);
  ClientApp* client = new ClientApp(config, args.windowConfig, args.serverConfig);
  bool renderServerPlayers = true;
  high_resolution_clock clock {};
  const uint64_t fixedDt = uint64_t(nanoseconds::period::den * config.physics.fixedDt);

  server->initialize();
  client->initialize();

  client->renderer.inject([&]() {
    if (renderServerPlayers)
    {
      uint64_t timeSinceLastUpdate = duration_cast<nanoseconds>(clock.now() - server->lastUpdateTime).count();
      float lerpFactor = float(timeSinceLastUpdate) / fixedDt;

      bool dw = client->renderer.drawWires;
      client->renderer.drawWires = true;

      for (int i = 0; i < server->scene.cars.capacity(); i++)
        if (server->scene.cars.isAlive(i))
        {
          const Car& car = server->scene.cars[i];
          client->renderer.drawCar(car, lerpFactor);
        }

      client->renderer.drawWires = dw;
    }
  });

  while (server->pulse() && client->pulse())
  {
    if (IsKeyPressed(KEY_U))
      renderServerPlayers = !renderServerPlayers;
  }

  client->shutdown();
  server->shutdown();

  delete client;
  delete server;
}

int main(int argc, char* argv[])
{
  Args args(argc, argv);

  const Config& config = Config::DEFAULT;

  switch (args.mode)
  {
    case Args::Server:
      runServer(args, config);
      return 0;

    case Args::Client:
      runClient(args, config);
      return 0;

    case Args::Test:
      runBoth(args, config);
      return 0;

    default:
      printf("Unexpected mode in args\n");
      return 0;
  }
}
