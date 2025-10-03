#include "Texture.h"
#include "pch.h"
#include <filesystem>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

namespace Engine
{

static bool IsDepthFormat(GLenum internalFormat, GLenum format)
{
    if (format == GL_DEPTH_COMPONENT)
        return true;
    switch (internalFormat)
    {
    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:
    case GL_DEPTH_COMPONENT32F:
    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
        return true;
    default:
        return false;
    }
}

Texture::Texture(const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        std::cerr << "[ERROR] Texture file not found: " << path << "\n";
        return;
    }

    int channelCount = 0;
    unsigned char* data = stbi_load(path.c_str(), &properties.width, &properties.height, &channelCount, 0);

    if (!data)
    {
        std::cerr << "[ERROR] Failed to load texture: " << path << "\n";
        return;
    }

    isLoaded = true;
    properties.target = GL_TEXTURE_2D;
    properties.layers = 1;

    switch (channelCount)
    {
    case 1:
        properties.internalFormat = GL_R8;
        properties.format = GL_RED;
        break;
    case 3:
        properties.internalFormat = GL_RGB8;
        properties.format = GL_RGB;
        break;
    case 4:
        properties.internalFormat = GL_RGBA8;
        properties.format = GL_RGBA;
        break;
    default:
        properties.internalFormat = GL_RGB8;
        properties.format = GL_RGB;
        break;
    }

    properties.pixels = data;
    customProperties.path = path;
    customProperties.mipmaps = true;
    customProperties.minFilter = GL_LINEAR_MIPMAP_LINEAR;

    create();

    stbi_image_free(data);
}

Texture::Texture(TextureProperties props, CustomProperties customProps)
    : properties(std::move(props)), customProperties(std::move(customProps))
{
    if (properties.layers <= 0)
        properties.layers = 1;
    create();
}

void Texture::create()
{
    glGenTextures(1, &id);
    glBindTexture(properties.target, id);

    const bool isDepth = IsDepthFormat(properties.internalFormat, properties.format);

    if (properties.target == GL_TEXTURE_2D_ARRAY)
    {
        // allocate array (pixels only valid for level 0 single layer uploads; here we allocate empty)
        glTexImage3D(GL_TEXTURE_2D_ARRAY, properties.level, properties.internalFormat, properties.width,
                     properties.height, properties.layers, properties.border, properties.format, properties.type,
                     properties.pixels);
    }
    else
    { // GL_TEXTURE_2D, etc.
        glTexImage2D(properties.target, properties.level, properties.internalFormat, properties.width,
                     properties.height, properties.border, properties.format, properties.type, properties.pixels);
    }

    if (isDepth)
    {
        glTexParameteri(properties.target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(properties.target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(properties.target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(properties.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(properties.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        if (properties.target == GL_TEXTURE_2D_ARRAY)
            glTexParameteri(properties.target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        const float border[4] = {1.f, 1.f, 1.f, 1.f};
        glTexParameterfv(properties.target, GL_TEXTURE_BORDER_COLOR, border);
    }
    else
    {
        if (customProperties.mipmaps)
            glGenerateMipmap(properties.target);
        glTexParameteri(properties.target, GL_TEXTURE_MIN_FILTER, customProperties.minFilter);
        glTexParameteri(properties.target, GL_TEXTURE_MAG_FILTER, customProperties.magFilter);
        glTexParameteri(properties.target, GL_TEXTURE_WRAP_S, customProperties.wrapS);
        glTexParameteri(properties.target, GL_TEXTURE_WRAP_T, customProperties.wrapT);
        if (properties.target == GL_TEXTURE_2D_ARRAY)
            glTexParameteri(properties.target, GL_TEXTURE_WRAP_R, customProperties.wrapR);
    }
}

void Texture::bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(properties.target, id);
}

void Texture::unbind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(properties.target, 0);
}

void Texture::resize(int w, int h)
{
    if (w <= 0 || h <= 0)
        return;

    if (w == properties.width && h == properties.height)
        return;

    properties.width = w;
    properties.height = h;

    bind();

    const bool isDepth = IsDepthFormat(properties.internalFormat, properties.format);

    const GLint level = 0;
    const GLint border = 0;

    if (properties.target == GL_TEXTURE_2D_ARRAY)
    {
        glTexImage3D(GL_TEXTURE_2D_ARRAY, level, properties.internalFormat, w, h, properties.layers, border,
                     properties.format, properties.type, nullptr);
    }
    else if (properties.target == GL_TEXTURE_CUBE_MAP)
    {
        for (int face = 0; face < 6; ++face)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, properties.internalFormat, w, h, border,
                         properties.format, properties.type, nullptr);
        }
    }
    else
    {
        glTexImage2D(properties.target, level, properties.internalFormat, w, h, border, properties.format,
                     properties.type, nullptr);
    }

    if (!isDepth && customProperties.mipmaps)
    {
        GLint minFilter = 0;
        glGetTexParameteriv(properties.target, GL_TEXTURE_MIN_FILTER, &minFilter);
        if (minFilter == GL_NEAREST_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR ||
            minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_LINEAR_MIPMAP_LINEAR)
        {
            glGenerateMipmap(properties.target);
        }
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

} // namespace Engine
