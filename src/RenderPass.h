#pragma once

#include "TextureManager.h"
#include "camera.h"
#include "pch.h"
#include <glad/glad.h>

class RenderPass
{
  public:
    RenderPass(bool renderToScreen);
    void render();
    Texture getResult();

  private:
    Texture m_texture;
    GLuint m_fbo;
    bool m_renderToScreen = false;
};