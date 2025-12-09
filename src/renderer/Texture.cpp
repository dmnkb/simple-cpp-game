#include <filesystem>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#include "pch.h"
#include "renderer/GLDebug.h"
#include "renderer/Texture.h"

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

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &id));
}

void Texture::create()
{
    GLCall(glGenTextures(1, &id));
    GLCall(glBindTexture(properties.target, id));

    const bool isDepth = IsDepthFormat(properties.internalFormat, properties.format);

    if (properties.target == GL_TEXTURE_2D_ARRAY)
    {
        GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, properties.level, properties.internalFormat, properties.width,
                            properties.height, properties.layers, properties.border, properties.format, properties.type,
                            properties.pixels));
    }
    else if (properties.target == GL_TEXTURE_CUBE_MAP_ARRAY)
    {
        // NOTE: properties.layers must be 6 * cubeCount (you already set this in your shadow pass)
        GLCall(glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, properties.level, properties.internalFormat, properties.width,
                            properties.height,
                            properties.layers, // 6 * numCubes
                            properties.border, properties.format, properties.type, properties.pixels));

        // On macOS, clamp mip levels if you’re not providing mipmaps:
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0));
        GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAX_LEVEL, 0));
    }
    else if (properties.target == GL_TEXTURE_CUBE_MAP)
    {
        // Allocate each face (in case you ever use non-array cubemaps here)
        for (int face = 0; face < 6; ++face)
        {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, properties.level, properties.internalFormat,
                                properties.width, properties.height, properties.border, properties.format,
                                properties.type, nullptr));
        }
    }
    else
    {
        GLCall(glTexImage2D(properties.target, properties.level, properties.internalFormat, properties.width,
                            properties.height, properties.border, properties.format, properties.type,
                            properties.pixels));
    }

    if (isDepth)
    {
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_COMPARE_MODE, GL_NONE));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        if (properties.target == GL_TEXTURE_2D_ARRAY || properties.target == GL_TEXTURE_CUBE_MAP_ARRAY)
            GLCall(glTexParameteri(properties.target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));
        const float border[4] = {1.f, 1.f, 1.f, 1.f};
        GLCall(glTexParameterfv(properties.target, GL_TEXTURE_BORDER_COLOR, border));
    }
    else
    {
        if (customProperties.mipmaps)
            GLCall(glGenerateMipmap(properties.target));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_MIN_FILTER, customProperties.minFilter));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_MAG_FILTER, customProperties.magFilter));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_WRAP_S, customProperties.wrapS));
        GLCall(glTexParameteri(properties.target, GL_TEXTURE_WRAP_T, customProperties.wrapT));
        if (properties.target == GL_TEXTURE_2D_ARRAY || properties.target == GL_TEXTURE_CUBE_MAP_ARRAY)
            GLCall(glTexParameteri(properties.target, GL_TEXTURE_WRAP_R, customProperties.wrapR));
    }
}

void Texture::bind(uint32_t slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(properties.target, id));
}

void Texture::unbind(uint32_t slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(properties.target, 0));
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
        GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, level, properties.internalFormat, w, h, properties.layers, border,
                            properties.format, properties.type, nullptr));
    }
    else if (properties.target == GL_TEXTURE_CUBE_MAP)
    {
        for (int face = 0; face < 6; ++face)
        {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, properties.internalFormat, w, h, border,
                                properties.format, properties.type, nullptr));
        }
    }
    else if (properties.target == GL_TEXTURE_CUBE_MAP_ARRAY)
    {
        GLCall(glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, level, properties.internalFormat, w, h, properties.layers,
                            border, properties.format, properties.type, nullptr));
    }

    else
    {
        GLCall(glTexImage2D(properties.target, level, properties.internalFormat, w, h, border, properties.format,
                            properties.type, nullptr));
    }

    if (!isDepth && customProperties.mipmaps)
    {
        GLint minFilter = 0;
        GLCall(glGetTexParameteriv(properties.target, GL_TEXTURE_MIN_FILTER, &minFilter));
        if (minFilter == GL_NEAREST_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR ||
            minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_LINEAR_MIPMAP_LINEAR)
        {
            GLCall(glGenerateMipmap(properties.target));
        }
    }
}

} // namespace Engine
