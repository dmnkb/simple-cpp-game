#include "RenderPass.h"
#include "TextureManager.h"
#include "Window.h"
#include "pch.h"

void RenderPass::bind(const Ref<Texture>& attachment)
{
    // Render to screen
    if (!attachment)
    {
        glColorMask(true, true, true, true);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::getFrameBufferDimensions().x, Window::getFrameBufferDimensions().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene ...
        return;
    }

    // Render to FBO
    glColorMask(false, false, false, false);

    if (!fbo)
        glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    attachment->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment->attachmentType, GL_TEXTURE_2D, attachment->id, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, attachment->texWidth, attachment->texHeight);

    // Render scene ...
}

void RenderPass::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderPass::~RenderPass()
{
    if (fbo)
        glDeleteFramebuffers(1, &fbo);
}