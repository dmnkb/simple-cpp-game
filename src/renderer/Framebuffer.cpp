#include <iostream>

#include "core/Window.h"
#include "renderer/Framebuffer.h"
#include "renderer/GLDebug.h"

namespace Engine
{

Framebuffer::Framebuffer()
{
    create();

    m_resizeHandle = EventManager::registerListeners(typeid(WindowReziseEvent).name(), [this](const Ref<Event> event)
                                                     { this->onFramebufferReziseEvent(event); });
}

Framebuffer::~Framebuffer()
{
    // Stop receiving resize events before destroying GL resources.
    // This prevents callbacks from firing on a partially/destructed object (use-after-free).
    EventManager::unregister(m_resizeHandle);
    m_resizeHandle = {};

    destroy();
}

void Framebuffer::create()
{
    GLCall(glGenFramebuffers(1, &m_fbo));
}

void Framebuffer::destroy()
{
    GLCall(glDeleteFramebuffers(1, &m_fbo));
}

void Framebuffer::bind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));

    // Collect all color attachments for glDrawBuffers
    bool hasDrawBuffers = false;
    std::vector<GLenum> drawBuffers;
    drawBuffers.reserve(m_attachments.size());

    for (const auto& attachment : m_attachments)
    {
        if (attachment->customProperties.attachmentType >= GL_COLOR_ATTACHMENT0 &&
            attachment->customProperties.attachmentType <= GL_COLOR_ATTACHMENT31)
        {
            drawBuffers.push_back(attachment->customProperties.attachmentType);
            hasDrawBuffers = true;
        }
    }

    // Assign draw buffers
    if (!drawBuffers.empty())
        GLCall(glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data()));
    else
        GLCall(glDrawBuffer(GL_NONE)); // Depth-only FBO

    // For depth-only FBOs, avoid accidental reads
    if (!hasDrawBuffers)
        GLCall(glReadBuffer(GL_NONE));

    // Match viewport to the first attachment (respect mip level)
    const auto& firstAttachment = m_attachments[0];
    const int firstLevel = firstAttachment->properties.level;
    m_width = firstAttachment->widthAtLevel(firstLevel);
    m_height = firstAttachment->heightAtLevel(firstLevel);
    GLCall(glViewport(0, 0, m_width, m_height));

    // Clear depth, optionally color
    GLbitfield clearMask = GL_DEPTH_BUFFER_BIT;
    if (hasDrawBuffers)
        clearMask |= GL_COLOR_BUFFER_BIT;

    GLCall(glClear(clearMask));
}

void Framebuffer::unbind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::attachTexture(const Ref<Texture>& attachment, std::string_view debugName)
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
    attachment->bind();

    const GLenum att = attachment->customProperties.attachmentType;

    if (attachment->properties.target == GL_TEXTURE_2D_ARRAY ||
        attachment->properties.target == GL_TEXTURE_CUBE_MAP_ARRAY || attachment->properties.target == GL_TEXTURE_3D)
    {
        // attach all layers at level 0; we’ll override the layer each draw via glFramebufferTextureLayer
        GLCall(glFramebufferTexture(GL_FRAMEBUFFER, att, attachment->id, 0));
    }
    else
    {
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, att, GL_TEXTURE_2D, attachment->id, 0));
    }

    m_attachments.push_back(attachment);

    GLenum status;
    GLCall(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "[Framebuffer] Incomplete! Status = 0x" << std::hex << status << std::dec << std::endl;
    else
        std::cout << "[Framebuffer] Successfully created FBO with attachment type 0x" << std::hex << att << std::dec
                  << " (" << debugName << ")" << std::endl;

    unbind();
}

void Framebuffer::reattachLayerForAll(GLint layer)
{
    // TODO: Check why this needs to be bound here, when the caller already does
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));

    for (const auto& a : m_attachments)
    {
        const GLenum attPoint = a->customProperties.attachmentType;
        const GLint level = a->properties.level;

        switch (a->properties.target)
        {
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        case GL_TEXTURE_CUBE_MAP_ARRAY:
            GLCall(glFramebufferTextureLayer(GL_FRAMEBUFFER, attPoint, a->id, level, std::max(0, layer)));
            break;

        default:
            // Non-array targets unchanged
            break;
        }
    }
}

glm::vec2 Framebuffer::getDimensions() const
{
    return glm::vec2(m_width, m_height);
};

void Framebuffer::onFramebufferReziseEvent(const Ref<Event> event)
{
    if (!dynamicSize)
        return;

    auto e = std::dynamic_pointer_cast<WindowReziseEvent>(event);
    if (!e)
        return;

    for (const auto& attachment : m_attachments)
        attachment->resize(e->windowWidth, e->windowHeight);
}

} // namespace Engine
