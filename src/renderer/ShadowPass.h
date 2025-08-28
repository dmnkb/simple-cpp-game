#pragma once

#include "Shader.h"
#include "scene/Scene.h"

namespace Engine
{

class ShadowPass
{
  public:
    ShadowPass();
    void execute(Scene& scene);

  private:
    Shader m_depthShader;
};

} // namespace Engine