#include "RenderPass.h"
#include "TextureManager.h"
#include "Window.h"
#include "pch.h"

RenderPass::RenderPass(bool renderToScreen) : m_renderToScreen(renderToScreen)
{
    if (renderToScreen)
        return;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    m_texture = TextureManager::loadTexture(Window::getFrameBufferDimensions());
    m_texture.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.id, 0);

    // Check if FBO is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer is not complete!" << std::endl;
}

void RenderPass::beginPass()
{
    if (m_renderToScreen)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getFrameBufferDimensions().x, Window::getFrameBufferDimensions().y);

        // Render scene ...

        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_texture.texWidth, m_texture.texHeight);

    // Render scene ...
}

Texture RenderPass::getResult()
{
    return m_texture;
}