#pragma once

#include "Texture.h"
#include <iostream>
#include <vector>

class Framebuffer
{
  public:
    Framebuffer();
    ~Framebuffer();

    void bind(const Ref<Texture>& attachment);
    void unbind();
    // void attachTexture(const Ref<Texture>& attachment);
    // void resize(int width, int height);
    // void reset();

  private:
    GLuint m_fbo = 0;
    // std::vector<Ref<Texture>> m_attachments;
};
