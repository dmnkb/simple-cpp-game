#pragma once

#include <assimp/material.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace Engine
{

struct TextureProperties
{
    GLenum target = GL_TEXTURE_2D;
    GLint level = 0;
    GLint internalFormat = GL_RGB8;
    GLsizei width = 1;
    GLsizei height = 1;
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
    GLenum minFilter = GL_LINEAR;
    GLenum magFilter = GL_LINEAR;
    std::string path;
    GLenum attachmentType = GL_COLOR_ATTACHMENT0;
    aiTextureType materialTextureType = aiTextureType_NONE;
};

struct Texture
{
    GLuint id = 0;
    bool isLoaded = 0;

    Texture() = default;
    Texture(const std::string& path);
    Texture(TextureProperties props, CustomProperties customProps = {});

    void bind(uint32_t slot = 0) const;
    void unbind(uint32_t slot = 0) const;
    void resize(int w, int h);

    ~Texture();

    TextureProperties properties;
    CustomProperties customProperties;

    void create();
};

} // namespace Engine