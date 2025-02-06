#pragma once

#include "RenderPass.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"

class ShadowPass : public RenderPass
{
  public:
    ShadowPass();
    void execute() override;

  private:
    Shader m_depthShader;
};