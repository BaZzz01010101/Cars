#include "pch.h"
#include "Helpers.h"
#include "DebugGraphs.h"

namespace game
{

  DebugGraph::DebugGraph(std::string name, Color color)
    : name(name), color(color), graph(GRAPH_SIZE, 0)
  {}

  void DebugGraph::logValue(float value)
  {
    graph[graphPos] = value;
    graphPos = (graphPos + 1) % GRAPH_SIZE;
    minValue = std::min(minValue, value);
    maxValue = std::max(maxValue, value);
  }

  float DebugGraph::getValue(int i)
  {
    return graph[(graphPos + i) % GRAPH_SIZE];
  }

  void DebugGraph::draw(Font font, float posX, float posY, float width, float height, bool pureGraph, const char* title)
  {
    if (!pureGraph)
      DrawRectangle(int(posX), int(posY), int(width), int(height), Fade(BLACK, 0.5f));

    float min = INFINITY;
    float max = -INFINITY;
    float avg = 0;

    for (float f : graph)
    {
      min = std::min(min, f);
      max = std::max(max, f);
      avg += f;
    }

    avg /= GRAPH_SIZE;

    if (avg < min || avg > max)
      avg = (min + max) / 2;

    minValue = moveTo(minValue, min, 0.1f * (min - minValue));
    maxValue = moveTo(maxValue, max, 0.1f * (max - maxValue));
    avgValue = moveTo(avgValue, avg, std::max(0.1f, 0.1f * fabsf(avg - avgValue)));
    float range = max - min;
    //range = std::max(range, 0.00001f);
    float stepY = range / height;
    float stepX = width / GRAPH_SIZE;

    for (int i = 0; i < GRAPH_SIZE; i++)
    {
      float v0 = getValue(i);
      float v1 = getValue(i + 1);

      float x1 = posX + i * stepX;
      float y1 = range == 0 ? 0 : mapRangeClamped(v0 - avg, -range, range, -height / 2, height / 2);
      float x2 = posX + (i + 1) * stepX;
      float y2 = range == 0 ? 0 : i < GRAPH_SIZE - 1 ? mapRangeClamped(v1 - avg, -range, range, -height / 2, height / 2) : y1;

      DrawLineV({ x1, posY + height / 2 - y1 }, { x2, posY + height / 2 - y2 }, Fade(color, 0.75f));
    }

    if (!pureGraph)
    {
      DrawTextEx(font, title ? title : name.c_str(), { posX, posY }, 20, 0, color);

      DrawTextEx(font, TextFormat("Avg: %.12f", avgValue), { posX, posY + height - 20 }, 20, 0, color);

      const char* maxText = TextFormat("Max: %.12f", maxValue);
      float maxTextWidth = MeasureTextEx(font, maxText, 20, 0).x;
      DrawTextEx(font, maxText, { posX + width - maxTextWidth, posY }, 20, 0, color);

      const char* minText = TextFormat("Min: %.12f", minValue);
      float minTextWidth = MeasureTextEx(font, minText, 20, 0).x;
      DrawTextEx(font, minText, { posX + width - maxTextWidth, posY + height - 20 }, 20, 0, color);
    }

    DrawLineV({ posX, posY + height / 2 }, { posX + width, posY + height / 2 }, Fade(WHITE, 0.25f));
  }

  void DebugGraph::reset()
  {
    for (float& f : graph)
      f = 0;

    graphPos = 0;
    minValue = 0;
    maxValue = 0;
    avgValue = 0;
  }

  void DebugGraphs::logValue(std::string name, Color color, float value)
  {
    DebugGraph* graph = nullptr;

    for (DebugGraph& g : graphs)
      if (g.name == name)
        graph = &g;

    if (!graph)
    {
      DebugGraph g(name, color);
      graphs.push_back(g);
      graph = &graphs.back();
    }

    graph->logValue(value);
  }

  DebugGraphs::DebugGraphs()
  {}

  void DebugGraphs::draw(Font font, float posX, float posY, float width, float height)
  {
    std::string selectedGraphTitle = graphs[selectedIndex].name;

    for (int di : drawingIndexes)
      if (di == selectedIndex)
      {
        selectedGraphTitle += " (added)";
        break;
      }

    graphs[selectedIndex].draw(font, posX, posY, width, height, false, selectedGraphTitle.c_str());

    for (int di : drawingIndexes)
      if (di != selectedIndex)
        graphs[di].draw(font, posX, posY, width, height, true, "");
  }

  bool DebugGraphs::get(const char* name, DebugGraph** graph)
  {
    for (DebugGraph& g : graphs)
      if (g.name == name)
      {
        *graph = &g;
        return true;
      }

    return false;
  }

  void DebugGraphs::selectNext()
  {
    selectedIndex = (selectedIndex + 1) % graphs.size();
  }

  void DebugGraphs::selectPrev()
  {
    selectedIndex = int((selectedIndex - 1 + graphs.size()) % graphs.size());
  }

  void DebugGraphs::updateControl()
  {
    // TODO: Fix potential incorrect controls
    // getting key state inside update can lead to incorrect calculations due to the multiple
    // accounting of the same key press in recurcive update calls in App::update because RayLib
    // seems updating input state once per frame
    if (IsKeyPressed(KEY_TAB) || IsKeyPressedRepeat(KEY_TAB))
    {
      if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))
        selectPrev();
      else
        selectNext();
    }

    if (IsKeyPressed(KEY_EQUAL))
    {
      bool containsSelected = false;

      for (int i : drawingIndexes)
        if (i == selectedIndex)
        {
          containsSelected = true;
          break;
        }

      if (!containsSelected)
        drawingIndexes.push_back(selectedIndex);
    }

    if (IsKeyPressed(KEY_MINUS))
    {
      for (int i = 0; i < drawingIndexes.size(); i++)
        if (drawingIndexes[i] == selectedIndex)
        {
          drawingIndexes.erase(drawingIndexes.begin() + i);
          break;
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE))
      drawingIndexes.clear();
  }

  void DebugGraphs::reset()
  {
    for (DebugGraph& g : graphs)
      g.reset();
  }

}