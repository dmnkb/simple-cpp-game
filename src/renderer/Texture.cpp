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

    // Determine internal and external format
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
    create();
}

void Texture::create()
{
    glGenTextures(1, &id);
    glBindTexture(properties.target, id);

    // Allocate / upload
    glTexImage2D(properties.target, properties.level, properties.internalFormat, properties.width, properties.height,
                 properties.border, properties.format, properties.type, properties.pixels);

    const bool isDepth = IsDepthFormat(properties.internalFormat, properties.format);

    if (isDepth)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const float border[4] = {1.f, 1.f, 1.f, 1.f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    }
    else
    {
        if (customProperties.mipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, customProperties.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, customProperties.magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, customProperties.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, customProperties.wrapT);
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
    if (w == properties.width && h == properties.height)
        return;

    properties.width = w;
    properties.height = h;

    bind();

    glTexImage2D(properties.target, properties.level, properties.internalFormat, w, h, properties.border,
                 properties.format, properties.type, properties.pixels);

    const bool isDepth = IsDepthFormat(properties.internalFormat, properties.format);

    if (!isDepth && customProperties.mipmaps)
        glGenerateMipmap(properties.target);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

} // namespace Engine
