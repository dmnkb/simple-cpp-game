#pragma once

#include "renderer/Renderer.h"
#include "scene/Scene.h"

namespace Engine
{

// TODO: Rename to editor

class DebugUI
{
  public:
    static void render(float fps, Scene& scene);
};

} // namespace Engine