#include "Framebuffer.h"
#include "Window.h"

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &m_fbo);
}

Framebuffer::~Framebuffer()
{
    reset();
}

void Framebuffer::bind(const Ref<Texture>& attachment)
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

    // Mask out all colors if attachment is a depth texture
    const bool isColorBuffer = attachment ? attachment->attachmentType == GL_COLOR_ATTACHMENT0 : true;
    glColorMask(isColorBuffer, isColorBuffer, isColorBuffer, isColorBuffer);

    if (!m_fbo)
        glGenFramebuffers(1, &m_fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    attachment->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment->attachmentType, GL_TEXTURE_2D, attachment->id, 0);

    // Check if FBO is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer is not complete!" << std::endl;

    // Render to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, attachment->texWidth, attachment->texHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render scene ...
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

void Framebuffer::resize(int width, int height)
{
    reset(); // Delete old framebuffer
    glGenFramebuffers(1, &m_fbo);

    for (auto& attachment : m_attachments)
    {
        // attachment->resize(width, height); // Resize attached textures
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment->attachmentType, GL_TEXTURE_2D, attachment->id, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer is not complete after resizing!" << std::endl;
}

void Framebuffer::reset()
{
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    m_attachments.clear();
}
