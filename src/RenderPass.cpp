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

    const bool colorAttachment = target == FRAMEBUFFER;

    texture = colorAttachment ? TextureManager::createColorTexture(Window::getFrameBufferDimensions())
                              : TextureManager::createDepthTexture(Window::getFrameBufferDimensions());

    std::cout << texture->id << std::endl;

    texture->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment ? GL_COLOR_ATTACHMENT0 : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           texture->id, 0);

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
    fbo = 0; // Reset FBO ID for safety

    if (texture)
    {
        // TODO: Should be done through the texture manager, but it's not properly set
        // up to be part of the cache!
        glDeleteTextures(1, &texture->id);
        texture.reset(); // Release the smart pointer
    }
}

Ref<Texture> RenderPass::getResult()
{
    return texture;
}