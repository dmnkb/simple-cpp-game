#pragma once

#include "RenderPass.h"
#include "Shader.h"
#include "pch.h"

class ShadowPass : public RenderPass
{
  public:
    ShadowPass();
    void execute() override;

  private:
    Shader m_depthShader;
    Shader m_depthShaderFoliage;
};