#pragma once

#include "Scene.h"
#include "Shader.h"

namespace Engine
{

class ShadowPass
{
  public:
    ShadowPass();
    void execute(Scene& scene, int& drawCalls);

  private:
    Shader m_depthShader;
};

} // namespace Engine