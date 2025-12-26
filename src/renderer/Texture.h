#pragma once

#include <assimp/material.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "assets/Asset.h"

namespace Engine
{

struct TextureProperties
{
    // Target can be: GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_3D, etc.
    GLenum target = GL_TEXTURE_2D;

    // Base mip level used when attaching
    GLint level = 0;

    // Base allocation size (mip 0)
    GLsizei width = 1;
    GLsizei height = 1;

    // For array/3D targets: number of layers (depth). Non-array: 1.
    GLsizei layers = 1;

    // Upload/format
    GLint internalFormat = GL_RGB8;
    GLint border = 0;
    GLenum format = GL_RGB;
    GLenum type = GL_UNSIGNED_BYTE;
    unsigned char* pixels = nullptr;
};

struct CustomProperties
{
    bool mipmaps = true;
    GLenum wrapS = GL_REPEAT;
    GLenum wrapT = GL_REPEAT;
    GLenum wrapR = GL_REPEAT;

    GLenum minFilter = GL_LINEAR;
    GLenum magFilter = GL_LINEAR;
    std::string path = "";

    // FBO attachment point (GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, ...)
    GLenum attachmentType = GL_COLOR_ATTACHMENT0;

    // PER-LAYER RENDERING DEFAULT:
    // For array/3D targets, the layer to attach. Default 0 = attach slice 0.
    // For non-array targets, ignored.
    GLint attachLayer = 0;

    aiTextureType materialTextureType = aiTextureType_NONE;
};

struct Texture : public Asset
{
    GLuint id = 0;
    bool isLoaded = false;

    Texture() = default;
    Texture(const std::string& path);
    Texture(TextureProperties props, CustomProperties customProps = {});

    // Identity
    AssetMetadata metadata{.type = AssetType::Texture, .name = "Unnamed Texture"};

    void bind(uint32_t slot = 0) const;
    void unbind(uint32_t slot = 0) const;

    // Resize keeps current layer count; for array/3D, changes width/height only.
    void resize(int w, int h);

    // Convenience: dimensions at mip (clamped)
    inline int widthAtLevel(int level) const { return std::max(1, properties.width >> level); }
    inline int heightAtLevel(int level) const { return std::max(1, properties.height >> level); }

    ~Texture();

    TextureProperties properties;
    CustomProperties customProperties;

    void create();
};

} // namespace Engine
