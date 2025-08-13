#pragma once

#include "renderer/Framebuffer.h"
#include "renderer/LightSceneNode.h"
#include "renderer/Material.h"
#include "scene/Scene.h"

namespace Engine
{

class LightingPass
{
  public:
    LightingPass();
    ~LightingPass();

    void execute(Scene& scene, int& drawCalls);
    Ref<Texture> getRenderTargetTexture()
    {
        return m_renderTargetTexture;
    }

  private:
    void updateUniforms(Scene& scene, const Ref<Material>& material);

  private:
    LightSceneNode::LightUBO m_lightBuffer[256];
    GLuint m_uboLights;

    Ref<Framebuffer> m_frameBuffer;
    Ref<Texture> m_renderTargetTexture;
};

} // namespace Engine