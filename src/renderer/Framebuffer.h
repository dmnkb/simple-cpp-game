#pragma once

#include "Texture.h"
#include "core/Core.h"
#include "core/Event.h"

namespace Engine
{

class Framebuffer
{
  public:
    // Interim solution to rezise framebuffers dynamically on window rezise.
    // TODO: Perhaps it's more elegant to create the framebuffer textures on the fly internally.
    bool dynamicSize = false;

    Framebuffer();
    ~Framebuffer();

    void create();
    void destroy();

    void bind();
    void unbind();
    void attachTexture(const Ref<Texture>& attachment);

    glm::vec2 getDimensions() const;

  private:
    void onFramebufferReziseEvent(const Ref<Event> event);

    GLuint m_fbo = 0;
    std::vector<Ref<Texture>> m_attachments;

    int m_width = 0;
    int m_height = 0;
};

} // namespace Engine