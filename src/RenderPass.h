#pragma once

#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"
#include <glad/glad.h>

class RenderPass
{
  public:
    RenderPass(const bool renderToScreen = true);
    void beginPass() const;
    Texture getResult();

  private:
    Texture m_texture;
    GLuint m_fbo;
    bool m_renderToScreen = false;
};