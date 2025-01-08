#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"
#include <glad/glad.h>

// TODO:
// Make this class only an abstraction of frame buffer objects that stores the textures
// and sets up the FBOs

enum ERenderFlags
{
    OPAQUE,
    TRANSLUCENT,
    IS_SCREEN_QUAD,
    CASTS_SHADOW,
};

// An atomic, stateless POD (plain old data) that contains mesh, material and transform.
struct Renderable
{
    Ref<Mesh> mesh;
    Ref<Shader> shader;
    glm::mat4 transform;
    std::unordered_set<ERenderFlags> flags;

    const bool isOpaque() const
    {
        return flags.find(OPAQUE) != flags.end();
    }

    const bool isTransparent() const
    {
        return flags.find(TRANSLUCENT) != flags.end();
    }

    const bool isScreenQuad() const
    {
        return flags.find(IS_SCREEN_QUAD) != flags.end();
    }

    const bool isCastsShadow() const
    {
        return flags.find(CASTS_SHADOW) != flags.end();
    }
};

enum ERenderTarget
{
    SCREEN,
    FRAMEBUFFER
};

using RenderPassFilter = std::function<bool(const Renderable&)>;

struct RenderPass
{
    void bind(const ERenderTarget& target);
    void unbind();

    Texture getResult();
    Texture texture;
    GLuint fbo;
};