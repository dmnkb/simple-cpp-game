#include "Framebuffer.h"
#include "Window.h"
#include <iostream>

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &m_fbo);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_fbo);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Collect all color attachments for glDrawBuffers
    std::vector<GLenum> drawBuffers;
    for (const auto& attachment : m_attachments)
    {
        if (attachment->customProperties.attachmentType >= GL_COLOR_ATTACHMENT0 &&
            attachment->customProperties.attachmentType <= GL_COLOR_ATTACHMENT31)
        {
            drawBuffers.push_back(attachment->customProperties.attachmentType);
        }
    }

    // Assign draw buffers
    if (!drawBuffers.empty())
        glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    else
        glDrawBuffer(GL_NONE); // Depth-only FBO

    // Match viewport to the first attachment
    if (!m_attachments.empty())
    {
        glViewport(0, 0, m_attachments[0]->properties.width, m_attachments[0]->properties.height);
    }

    // Clear depth, optionally color
    GLbitfield clearMask = GL_DEPTH_BUFFER_BIT;
    for (const auto& attachment : m_attachments)
    {
        if (attachment->customProperties.attachmentType >= GL_COLOR_ATTACHMENT0 &&
            attachment->customProperties.attachmentType <= GL_COLOR_ATTACHMENT31)
        {
            clearMask |= GL_COLOR_BUFFER_BIT;
            break;
        }
    }

    glClear(clearMask);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTexture(const Ref<Texture>& attachment)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    attachment->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment->customProperties.attachmentType, GL_TEXTURE_2D, attachment->id,
                           0);

    m_attachments.push_back(attachment);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "[Framebuffer] Incomplete! Status = 0x" << std::hex << status << std::dec << std::endl;
    }

    unbind();
}
