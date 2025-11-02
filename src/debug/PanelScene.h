#pragma once

#include "renderer/Renderer.h"
#include "scene/Scene.h"

namespace Engine
{

class PanelScene
{
  public:
    static void render(const float fps, Scene& scene);
};

} // namespace Engine