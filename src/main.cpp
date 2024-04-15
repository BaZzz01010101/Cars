#include "core.h"
#include "App.h"
#include "ServerApp.h"

using namespace game;

int main(int argc, char* argv[])
{
  if (argc > 1 && !strcmp(argv[1], "-test"))
  {
    std::thread app([]() {
      App* app = new App();

      app->initialize();
      app->run();
      app->shutdown();

      delete app;
    });

    volatile bool* serverExit = nullptr;

    std::thread server([&]() {
      ServerApp* app = new ServerApp();
      serverExit = &app->exit;

      app->initialize();
      app->run();
      app->shutdown();

      delete app;
    });

    app.join();

    if (!serverExit)
      server.detach();
    else
    {
      *serverExit = true;
      server.join();
    }
  }
  else if (argc > 1 && !strcmp(argv[1], "-server"))
  {
    ServerApp* app = new ServerApp();

    app->initialize();
    app->run();
    app->shutdown();

    delete app;
  }
  else
  {
    App* app = new App();

    app->initialize();
    app->run();
    app->shutdown();

    delete app;
  }

  return 0;
}
