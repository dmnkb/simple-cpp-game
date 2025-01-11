#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "MeshSceneNode.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"
#include <glad/glad.h>

// TODO:
// Make this class only an abstraction of frame buffer objects that stores the textures
// and sets up the FBOs

enum ERenderTarget
{
    SCREEN,
    FRAMEBUFFER,
    FRAMEBUFFER_DEPTH,
};

struct RenderPass
{
    void bind(const ERenderTarget& target, const bool isDepthPass = false);
    void unbind();

    Ref<Texture> getResult();
    Ref<Texture> texture;
    GLuint fbo = 0;
};