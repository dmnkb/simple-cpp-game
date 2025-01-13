#include "RenderPass.h"
#include "TextureManager.h"
#include "Window.h"
#include "pch.h"

void RenderPass::bind(const ERenderTarget& target, const bool isDepthPass)
{
    // TODO: Check if correct
    glColorMask(!isDepthPass, !isDepthPass, !isDepthPass, !isDepthPass);

    // Render to screen
    if (target == SCREEN)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getFrameBufferDimensions().x, Window::getFrameBufferDimensions().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene ...
        return;
    }

    // Render to FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    texture = TextureManager::createColorTexture(Window::getFrameBufferDimensions());
    texture->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0);

    // Check if FBO is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer is not complete!" << std::endl;

    // Render to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, texture->texWidth, texture->texHeight);

    // Render scene ...
}

void RenderPass::unbind()
{
    glDeleteFramebuffers(1, &fbo);
    fbo = 0;
}

Ref<Texture> RenderPass::getResult()
{
    return texture;
}