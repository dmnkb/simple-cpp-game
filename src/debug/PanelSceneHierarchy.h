#pragma once

#include "renderer/Renderer.h"
#include "scene/Scene.h"

namespace Engine
{

class PanelSceneHierarchy
{
  public:
    static void render(const Scene& scene);
};

} // namespace Engine