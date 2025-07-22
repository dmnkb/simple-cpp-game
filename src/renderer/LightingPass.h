#pragma once

#include "Framebuffer.h"
#include "LightSceneNode.h"
#include "Material.h"
#include "Scene.h"
#include "pch.h"

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