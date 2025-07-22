#pragma once

#include "Renderer.h"
#include "Scene.h"

class PanelStats
{
  public:
    static void render(const float fps, const std::map<std::string, int>& drawCallsPerPass, const Scene& scene);
};
