#pragma once

#include "LightSceneNode.h"
#include "Material.h"
#include "RenderPass.h"
#include "pch.h"

class ForwardPass : public RenderPass
{
  public:
    ForwardPass();
    ~ForwardPass();
    void execute();

  private:
    void initLightUBO();
    void updateLightUBO();

    void setViewUniforms(const Ref<Material>& material);
    void setLightUniforms(const Ref<Material>& material);
    void setShadowUniforms(const Ref<Material>& material);

  private:
    LightSceneNode::LightUBO m_lightBuffer[256];
    GLuint m_uboLights;
};