#pragma once

#include "LightingPass.h"
#include "PostProcessingPass.h"
#include "ShadowPass.h"
#include "scene/Scene.h"

namespace Engine
{

class Renderer
{
  public:
    Renderer();
    ~Renderer();

    void update(Scene& scene);

  private:
    ShadowPass m_shadowPass;
    LightingPass m_lightingPass;
    PostProcessingPass m_postProcessingPass;
};

} // namespace Engine