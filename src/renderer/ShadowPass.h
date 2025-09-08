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

    // Renders/updates shadow maps for all spot lights and returns handles to the textures.
    ShadowOutputs execute(Scene& scene); // <-- return by value

  private:
    void ensureFBOForSpotCount(int spotCount);
    void recreateFBO(int layers, int width, int height, bool withDebug = false);

  private:
    Shader m_depthShader;
    Ref<Framebuffer> m_shadowFramebuffer;

    // Owned GPU resources (stable lifetime across frames)
    Ref<Texture> m_depthArray;      // GL_TEXTURE_2D_ARRAY, depth
    Ref<Texture> m_debugColorArray; // optional GL_TEXTURE_2D_ARRAY, rgba

    // State tracking
    int m_shadowRes = 1024;
    int m_allocLayers = 0;
    bool m_withDebug = false;
};

} // namespace Engine
