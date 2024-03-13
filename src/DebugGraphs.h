#pragma once

namespace game
{

  struct DebugGraph
  {
    static constexpr int GRAPH_SIZE = 256;
    std::string name;
    std::vector<float> graph;
    Color color = WHITE;
    int graphPos = 0;
    float minValue = 0;
    float maxValue = 0;
    float avgValue = 0;

    DebugGraph(std::string name, Color color);

    void logValue(float value);

    float getValue(int i);

    void draw(Font font, float posX, float posY, float width, float height, bool pureGraph, const char* title = nullptr);

    void reset();
  };

  struct DebugGraphs
  {
    std::vector<DebugGraph> graphs;
    std::vector<int> drawingIndexes;
    int selectedIndex = 0;

    DebugGraphs();

    void logValue(std::string name, Color color, float value);

    void draw(Font font, float posX, float posY, float width, float height);

    bool get(const char* name, DebugGraph** graph);

    void selectNext();

    void selectPrev();

    void updateControl();

    void reset();
  };

}