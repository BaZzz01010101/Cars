#include "pch.h"
#include "App.h"

using namespace game;

static App* app = nullptr;

int main()
{
  app = new App();

  app->initialize();
  app->run();
  app->shutdown();

  delete app;

  return 0;
}
