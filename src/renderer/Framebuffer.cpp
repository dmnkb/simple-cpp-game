#include "Framebuffer.h"
#include "Window.h"

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &m_fbo);
}

Framebuffer::~Framebuffer()
{
    // reset();
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    if (!m_attachments.empty())
    {
        // Determine if there are color attachments
        std::vector<GLenum> drawBuffers;
        for (size_t i = 0; i < m_attachments.size(); ++i)
        {
            if (m_attachments[i]->format != GL_DEPTH_COMPONENT) // Avoid setting draw buffers for depth-only textures
                drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        // Only set draw buffers if color attachments exist
        if (!drawBuffers.empty())
            glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
        else
            glDrawBuffer(GL_NONE); // Disable color writing if no color attachments exist
        // Set viewport to match the first attachment
        glViewport(0, 0, m_attachments[0]->texWidth, m_attachments[0]->texHeight);
    }
    // Clear framebuffer
    GLbitfield clearMask = GL_DEPTH_BUFFER_BIT; // Always clear depth
    if (!m_attachments.empty() && m_attachments[0]->format != GL_DEPTH_COMPONENT)
        clearMask |= GL_COLOR_BUFFER_BIT; // Only clear color buffer if there are color attachments
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment->attachmentType, GL_TEXTURE_2D, attachment->id, 0);
    m_attachments.push_back(attachment);

    // Update draw buffers
    std::vector<GLenum> drawBuffers;
    for (size_t i = 0; i < m_attachments.size(); ++i)
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);

    glDrawBuffers(drawBuffers.size(), drawBuffers.data());

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// void Framebuffer::resize(int width, int height)
// {
//     reset(); // Delete old framebuffer
//     glGenFramebuffers(1, &m_fbo);

//     for (auto& attachment : m_attachments)
//     {
//         // attachment->resize(width, height); // Resize attached textures
//         glFramebufferTexture2D(GL_FRAMEBUFFER, attachment->attachmentType, GL_TEXTURE_2D, attachment->id, 0);
//     }

//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//         std::cerr << "Framebuffer is not complete after resizing!" << std::endl;
// }

// void Framebuffer::reset()
// {
//     if (m_fbo)
//     {
//         glDeleteFramebuffers(1, &m_fbo);
//         m_fbo = 0;
//     }
//     m_attachments.clear();
// }
