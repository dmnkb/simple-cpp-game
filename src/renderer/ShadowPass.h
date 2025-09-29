#pragma once

#include "Framebuffer.h"
#include "PassIO.h"
#include "Shader.h"
#include "Texture.h"
#include "scene/Scene.h"

namespace Engine
{
constexpr int MAX_SPOTLIGHT_SHADOW_COUNT = 16;

class ShadowPass
{
  public:
    ShadowPass();

    ShadowOutputs execute(Scene& scene);

  private:
    void ensureFBOForSpotCount(int spotCount);
    void recreateFBO(int layers, int width, int height, bool withDebug = false);

  private:
    Shader m_depthShader;
    Ref<Framebuffer> m_shadowFramebuffer;
    Ref<Texture> m_depthArray;

    // State tracking
    int m_shadowRes = 1024;
    int m_allocLayers = 0;
};

} // namespace Engine
