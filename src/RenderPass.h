#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "MeshSceneNode.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"
#include <glad/glad.h>

struct RenderPass
{
    void bind(const Ref<Texture>& attachment = nullptr);
    void unbind();

    ~RenderPass();

    GLuint fbo = 0;
};