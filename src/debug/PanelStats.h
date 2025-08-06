#pragma once

#include "Renderer.h"
#include "Scene.h"

namespace Engine
{

class PanelStats
{
  public:
    static void render(const float fps, const std::map<std::string, int>& drawCallsPerPass, const Scene& scene);
};

} // namespace Engine