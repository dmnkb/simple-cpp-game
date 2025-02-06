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
    void updateUniforms(const Ref<Material>& material);

  private:
    LightSceneNode::LightUBO m_lightBuffer[256];
    GLuint m_uboLights;
};