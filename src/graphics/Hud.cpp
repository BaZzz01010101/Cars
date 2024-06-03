#include "core.h"
#include "Hud.h"

namespace game
{
  Hud::Hud(const Config& config, const MatchStats& matchStats) :
    config(config),
    matchStats(matchStats)
  {
  }

  Hud::~Hud()
  {
    if (crosshairsTextureLoaded)
      UnloadTexture(crosshairsTexture);
  }

  void Hud::init()
  {
    font = LoadFontEx(config.graphics.resources.fontPath, config.graphics.hud.fontSize, 0, 0);
    crosshairsTexture = LoadTexture(config.graphics.resources.crosshairsTexturePath);
    crosshairsTextureLoaded = true;
    lastColor = WHITE;
    lastPosX = config.graphics.hud.screenMargins;
    lastPosY = config.graphics.hud.screenMargins;
    lastFontSize = config.graphics.hud.fontSize;
  }

  void Hud::update()
  {
    debugGraphs.updateControl();
  }

  void Hud::draw(const CustomCamera& camera, const Scene& scene, float lerpFactor) const
  {
    if (scene.localPlayerIndex < 0)
      return;

    drawCrossHairs(camera, scene, lerpFactor);
    drawCountdown(scene);
    drawLocalPlayerHealth(scene);
    drawMatchStats(scene);
    drawMatchTimer(scene);

    if (drawDebugInfo)
      drawDebug(scene);
  }

  void Hud::print(const char* text) const
  {
    print(text, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* text, Color color) const
  {
    print(text, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* text, Color color, int posX) const
  {
    print(text, lastColor, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* text, Color color, int posX, int posY) const
  {
    print(text, color, posX, posY, lastFontSize);
  }

  // TODO: change type of x, y, and fontSize to float
  void Hud::print(const char* text, Color color, int posX, int posY, int fontSize) const
  {
    DrawTextEx(font, text, { (float)posX - 1, (float)posY - 1 }, (float)fontSize, 0, BLACK);
    DrawTextEx(font, text, { (float)posX + 1, (float)posY - 1 }, (float)fontSize, 0, BLACK);
    DrawTextEx(font, text, { (float)posX - 1, (float)posY + 1 }, (float)fontSize, 0, BLACK);
    DrawTextEx(font, text, { (float)posX + 1, (float)posY + 1 }, (float)fontSize, 0, BLACK);
    DrawTextEx(font, text, { (float)posX, (float)posY }, (float)fontSize, 0, color);

    lastColor = color;
    lastPosX = posX;
    lastPosY = posY;
    lastFontSize = fontSize;
  }

  void Hud::printIf(bool condition, const char* text) const
  {
    printIf(condition, text, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color) const
  {
    printIf(condition, text, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color, int posX) const
  {
    printIf(condition, text, color, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color, int posX, int posY) const
  {
    printIf(condition, text, color, posX, posY, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color, int posX, int posY, int fontSize) const
  {
    if (condition)
      print(text, color, posX, posY, fontSize);
    else
    {
      lastColor = color;
      lastPosX = posX;
      lastPosY = posY;
      lastFontSize = fontSize;
    }
  }

  void Hud::print(const char* title, vec3 v) const
  {
    print(title, v, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color) const
  {
    print(title, v, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color, int posX) const
  {
    print(title, v, color, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color, int posX, int posY) const
  {
    print(title, v, color, posX, posY, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color, int posX, int posY, int fontSize) const
  {
    print(TextFormat("%-10s: %5.1f |%5.1f |%5.1f :%5.1f", title, v.x, v.y, v.z, v.length()), color, posX, posY, fontSize);

    if (!paused)
    {
      debugGraphs.logValue(std::string(title) + ".x", color, v.x);
      debugGraphs.logValue(std::string(title) + ".y", color, v.y);
      debugGraphs.logValue(std::string(title) + ".z", color, v.z);
      debugGraphs.logValue(std::string(title) + ".len", color, v.length());
    }
  }

  void Hud::print(const char* title, float f) const
  {
    print(title, f, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color) const
  {
    print(title, f, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color, int posX) const
  {
    print(title, f, color, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color, int posX, int posY) const
  {
    print(title, f, color, posX, posY, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color, int posX, int posY, int fontSize) const
  {
    print(TextFormat("%-10s: %5.1f", title, f), color, posX, posY, fontSize);

    if (!paused)
      debugGraphs.logValue(title, color, f);
  }

  void Hud::drawCrossHairs(const CustomCamera& camera, const Scene& scene, float lerpFactor) const
  {
    int screenWidth = config.graphics.screen.width;
    int screenHeight = config.graphics.screen.height;
    float minScreenSize = float(std::min(screenWidth, screenHeight));
    float size = (camera.mode == CustomCamera::Zoom) ? (minScreenSize / 6) : (minScreenSize / 12);
    Color color = { 255, 255, 255, 196 };

    vec2 center = { float(screenWidth / 2), float(screenHeight / 2) };
    drawCrossHair(center, CrossHairIndex::Camera, size, color);

    const Car& player = scene.getLocalPlayer();

    drawTurretCrossHair(camera, scene, player.cannon, CrossHairIndex::Cannon, size, color, lerpFactor);
    drawTurretCrossHair(camera, scene, player.gun, CrossHairIndex::Gun, size, color, lerpFactor);
  }

  void Hud::drawTurretCrossHair(const CustomCamera& camera, const Scene& scene, const Turret& turret, CrossHairIndex crosshairIndex, float size, Color color, float lerpFactor) const
  {
    if (camera.direction * turret.forward() > 0)
    {
      vec3 barrelBack = turret.barrelBackPosition(lerpFactor);
      float distanceToTarget = barrelBack.distanceTo(turret.expectedTarget);
      vec3 target;

      if (!scene.traceRay(barrelBack, turret.forward(), FLT_MAX, scene.localPlayerIndex, &target, nullptr, nullptr, nullptr))
        target = barrelBack + turret.forward() * distanceToTarget;

      vec2 position = GetWorldToScreen(target, camera);

      // Fixes bug in RayLib with GetWorldToScreen returning NaN in some cases
      // Known case is when turret.currentTarget == camera.position
      if (position == position)
      {
        crossHairPositions[crosshairIndex] = moveToRelative(crossHairPositions[crosshairIndex], position, CROSSHAIR_MOVEMENT_SHARPNESS);
        position = crossHairPositions[crosshairIndex];
        drawCrossHair(position, crosshairIndex, size, color);
      }
    }
  }

  void Hud::drawCrossHair(vec2 position, CrossHairIndex crosshairIndex, float size, Color color) const
  {
    float textureSize = (float)crosshairsTexture.height;
    float textureLeft = textureSize * (float)crosshairIndex;
    DrawTexturePro(crosshairsTexture, { textureLeft, 0, textureSize, textureSize }, { float(position.x - 0.5f * size), float(position.y - 0.5f * size), size, size }, { 0, 0 }, 0, color);
  }

  void Hud::drawCountdown(const Scene& scene) const
  {
    static const Config::Graphics::Screen& screen = config.graphics.screen;
    static const int fontSize = 200;
    static const vec2 charSize = MeasureTextEx(font, "0", fontSize, 0);
    static const int x = (screen.width - (int)charSize.x) / 2;
    static const int y = (screen.height - (int)charSize.y) / 4;

    const Car& car = scene.getLocalPlayer();

    if (car.aliveState == Car::Countdown && scene.matchState == Scene::Running)
    {
      const char* text = TextFormat("%i", int(1 + car.getAliveStateTimeout()));
      print(text, WHITE, x, y, fontSize);
    }
    else if (scene.matchState == Scene::Countdown)
    {
      const char* text = TextFormat("%i", int(1 + scene.getMatchStateTimeout()));
      print(text, WHITE, x, y, fontSize);
    }
  }

  void Hud::drawLocalPlayerHealth(const Scene& scene) const
  {
    static const Config::Graphics::Screen& screen = config.graphics.screen;
    const Car& car = scene.getLocalPlayer();

    if (IS_VERTICAL_HEALTH_BAR)
    {
      static const int width = std::min(screen.width, screen.height) / 30;
      static const int height = screen.height / 3;
      // TODO: use hud margins from config
      static const int left = 20;
      static const int top = screen.height - height - 20;
      int hpHeight = height * car.health / config.physics.car.maxHealth;

      DrawRectangle(left, top, width, height - hpHeight, DARKGRAY);
      DrawRectangle(left, top + height - hpHeight, width, hpHeight, RED);
    }
    else
    {
      static const int width = screen.width / 4;
      static const int height = std::min(screen.width, screen.height) / 30;
      // TODO: use hud margins from config
      static const int left = 20;
      static const int top = screen.height - height - 20;
      int hpWidth = width * car.health / config.physics.car.maxHealth;

      DrawRectangle(left, top, hpWidth, height, RED);
      DrawRectangle(left + hpWidth, top, width - hpWidth, height, DARKGRAY);
    }
  }

  void Hud::drawMatchStats(const Scene& scene) const
  {
    static const Config::Graphics::Screen& screen = config.graphics.screen;
    static constexpr int BUF_SIZE = 256;
    static char title[BUF_SIZE];
    static int dummy = snprintf(title, BUF_SIZE, "%-24s %-6s %-7s %-4s", "Name", "Kills", "Deaths", "Ping");
    int x, y, fontSize;

    if (scene.matchState == Scene::Scoreboard)
    {
      fontSize = 40;
      static const int textWidth = (int)MeasureTextEx(font, title, (float)fontSize, 0).x;
      // TODO: use hud margins from config
      x = std::max(20, (screen.width - textWidth) / 2);
      y = screen.height / 4;
    }
    else
    {
      fontSize = 20;
      static const int textWidth = (int)MeasureTextEx(font, title, (float)fontSize, 0).x;
      // TODO: use hud margins from config
      x = screen.width - textWidth - 20;
      y = 20;
    }

    print(title, LIGHTGRAY, x, y, fontSize);

    for (const PlayerStats& ps : matchStats.playerStats)
    {
      Color color = (ps.guid == scene.localPlayerGuid) ? YELLOW : WHITE;

      if (const Car* player = scene.tryGetPlayer(ps.guid))
      {
        const char* line = TextFormat("%-24s %-6i %-7i %-4i", player->name, ps.kills, ps.deaths, ps.ping);
        print(line, color);
      }
    }
  }

  void Hud::drawMatchTimer(const Scene& scene) const
  {
    static const Config::Graphics::Screen& screen = config.graphics.screen;
    // TODO: use hud margins from config
    static const int y = 20;

    if (scene.matchState == Scene::Scoreboard)
    {
      float matchStateTimeout = scene.getMatchStateTimeout();
      int mins = int(matchStateTimeout / 60);
      int secs = int(matchStateTimeout - mins * 60);
      const char* text = TextFormat("Time To next match: %02i:%02i", mins, secs);
      static const int fontSize = 30;
      static const int textWidth = (int)MeasureTextEx(font, text, fontSize, 0).x;
      static const int x = (screen.width - textWidth) / 2;
      print(text, WHITE, x, y, fontSize);
    }
    else if (scene.isWaitingForPlayers())
    {
      static const char* text = "Waiting for players";
      static const int fontSize = 30;
      static const int textWidth = (int)MeasureTextEx(font, text, fontSize, 0).x;
      static const int x = (screen.width - textWidth) / 2;
      print(text, WHITE, x, y, fontSize);
    }
    else if(scene.matchState == Scene::Running)
    {
      float matchStateTimeout = scene.getMatchStateTimeout();
      int mins = int(matchStateTimeout / 60);
      int secs = int(matchStateTimeout - mins * 60);
      const char* text = TextFormat("%02i:%02i", mins, secs);
      static const int fontSize = 60;
      static const int textWidth = (int)MeasureTextEx(font, text, fontSize, 0).x;
      static const int x = (screen.width - textWidth) / 2;
      print(text, WHITE, x, y, fontSize);
    }
  }

  void Hud::drawDebug(const Scene& scene) const
  {
    lastFontSize = 20;

    if (paused)
      // TODO: use hud margins from config
      print("Paused", YELLOW, 10, 10);
    else
      DrawFPS(10, 10);

    const Car& player = scene.getLocalPlayer();
    // TODO: use hud margins from config
    print("Position", player.position, LIGHTGRAY, 10, 30);
    vec3 rotation;
    player.rotation.toEuler(&rotation.y, &rotation.z, &rotation.x);
    print("Rotation", rotation, GRAY);
    print("Force", player.force, RED);
    print("Moment", player.moment, BLUE);
    print("Velocity", player.velocity, GREEN);
    print("Engine Pow.", player.enginePower, YELLOW);

    print("Angular velocity", player.angularVelocity, BLUE);
    print("Wheel 1 fr.force", player.frontLeftWheel.frictionForce.length(), LIGHTGRAY);
    print("Wheel 2 fr.force", player.frontRightWheel.frictionForce.length(), LIGHTGRAY);
    print("Wheel 3 fr.force", player.rearLeftWheel.frictionForce.length(), LIGHTGRAY);
    print("Wheel 4 fr.force", player.rearRightWheel.frictionForce.length(), LIGHTGRAY);

    print("Cars: ", (float)scene.cars.count(), GREEN);
    print("Projectiles: ", (float)scene.projectiles.count(), GREEN);
    print("Explosion particles: ", (float)scene.explosionParticles.count(), GREEN);

    print("Trace count:", (float)scene.terrain.traceCount, WHITE);
    print("Players", (float)scene.cars.count(), LIGHTGRAY);
    print("Physical Frame Offset", (float)scene.localPhysicalFrame - scene.serverPhysicalFrame, LIGHTGRAY);

  }
}