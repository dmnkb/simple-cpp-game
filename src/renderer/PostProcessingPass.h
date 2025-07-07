#pragma once

#include "LightSceneNode.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "pch.h"

class PostProcessingPass
{
  public:
    PostProcessingPass();
    void execute(Scene& scene, const Ref<Texture>& renderTargetTexture);

  private:
    void initQuad();
    void updateUniforms(Scene& scene, const Ref<Material>& material);

    Shader m_postProcessShader;

    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;
};