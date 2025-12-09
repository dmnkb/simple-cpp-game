#pragma once

#include "renderer/PassIO.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "scene/Scene.h"

namespace Engine
{

class PostProcessingPass
{
  public:
    PostProcessingPass();
    void execute(const PostProcessingInputs& postProcessingInputs);

  private:
    Shader m_postProcessShader;

    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;
};

} // namespace Engine