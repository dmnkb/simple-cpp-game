#pragma once

#include "Renderer.h"
#include "Scene.h"

class DebugUI
{
  public:
    static void render(float fps, const std::map<std::string, int>& drawCallsPerPass, const Scene& scene);
};
