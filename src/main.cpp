#include "pch.h"
#include "App.h"

using namespace game;

int main()
{
  App app{};
  app.initialize();
  app.run();
  app.shutdown();

  return 0;
}
