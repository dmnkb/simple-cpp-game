#pragma once

#include "renderer/Renderer.h"
#include "scene/Scene.h"

namespace Engine
{

class DebugUI
{
  public:
    static void render(float fps, const Scene& scene);
};

} // namespace Engine