#pragma once

#include "Texture.h"
#include "core/Core.h"
#include "core/Event.h"

namespace Engine
{

class Framebuffer
{
  public:
    // Interim solution to resize framebuffers dynamically on window resize.
    bool dynamicSize = false;

    Framebuffer();
    ~Framebuffer();

    void create();
    void destroy();

    void bind();
    void unbind();

    // Attach a texture (2D or array/3D). Uses Texture::properties.level and CustomProperties::attachLayer.
    void attachTexture(const Ref<Texture>& attachment);

    // Re-attach all array/3D attachments to a specific slice (per-layer rendering).
    void reattachLayerForAll(GLint layer);

    glm::vec2 getDimensions() const;

  private:
    void onFramebufferReziseEvent(const Ref<Event> event);

    GLuint m_fbo = 0;
    std::vector<Ref<Texture>> m_attachments;

    int m_width = 0;
    int m_height = 0;
};

} // namespace Engine
