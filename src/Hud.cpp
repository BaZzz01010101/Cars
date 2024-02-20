#include "pch.h"
#include "Hud.h"

namespace game
{
  void Hud::init(const Config& config)
  {
    hudConfig = config.graphics.hud;
    screenConfig = config.graphics.screen;
    font = LoadFontEx(config.graphics.resources.fontPath, hudConfig.fontSize, 0, 0);
    crosshairsTexture = LoadTexture(config.graphics.resources.crosshairsTexturePath);
    lastColor = WHITE;
    lastPosX = hudConfig.screenMargins;
    lastPosY = hudConfig.screenMargins;
    lastFontSize = hudConfig.fontSize;
  }

  void Hud::print(const char* text)
  {
    print(text, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* text, Color color)
  {
    print(text, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* text, Color color, int posX)
  {
    print(text, lastColor, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* text, Color color, int posX, int posY)
  {
    print(text, color, posX, posY, lastFontSize);
  }

  void Hud::print(const char* text, Color color, int posX, int posY, int fontSize)
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

  void Hud::printIf(bool condition, const char* text)
  {
    printIf(condition, text, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color)
  {
    printIf(condition, text, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color, int posX)
  {
    printIf(condition, text, color, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color, int posX, int posY)
  {
    printIf(condition, text, color, posX, posY, lastFontSize);
  }

  void Hud::printIf(bool condition, const char* text, Color color, int posX, int posY, int fontSize)
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

  void Hud::print(const char* title, vec3 v)
  {
    print(title, v, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color)
  {
    print(title, v, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color, int posX)
  {
    print(title, v, color, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color, int posX, int posY)
  {
    print(title, v, color, posX, posY, lastFontSize);
  }

  void Hud::print(const char* title, vec3 v, Color color, int posX, int posY, int fontSize)
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

  void Hud::print(const char* title, float f)
  {
    print(title, f, lastColor, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color)
  {
    print(title, f, color, lastPosX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color, int posX)
  {
    print(title, f, color, posX, lastPosY + lastFontSize, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color, int posX, int posY)
  {
    print(title, f, color, posX, posY, lastFontSize);
  }

  void Hud::print(const char* title, float f, Color color, int posX, int posY, int fontSize)
  {
    print(TextFormat("%-10s: %5.1f", title, f), color, posX, posY, fontSize);

    if (!paused)
      debugGraphs.logValue(title, color, f);
  }

  void Hud::update()
  {
    debugGraphs.updateControl();
  }

  void Hud::draw(const Scene& scene)
  {
    drawCrosshairs(scene);
    drawDebug(scene);
  }

  void Hud::drawCrosshairs(const Scene& scene)
  {
    Vector2 cameraCrosshairPosition{ float(screenConfig.width) / 2, float(screenConfig.height) / 2 };
    float crosshairSrcSize = (float)crosshairsTexture.height;
    float crosshairDstSize = (float)std::min(screenConfig.width, screenConfig.height) / 16;

    DrawTexturePro(crosshairsTexture, { 0, 0, crosshairSrcSize, crosshairSrcSize }, { float(screenConfig.width / 2 - crosshairDstSize / 2), float(screenConfig.height / 2 - crosshairDstSize / 2), crosshairDstSize, crosshairDstSize }, { 0, 0 }, 0, WHITE);
  }



  void Hud::drawDebug(const Scene& scene)
  {
    if (paused)
      print("Paused", YELLOW, 10, 10);
    else
      DrawFPS(10, 10);

    const Car& player = scene.getPlayer();
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
  }
}