#include "Texture.h"
#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#include <filesystem>
#include <iostream>

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

    glTexImage2D(properties.target, properties.level, properties.internalFormat, properties.width, properties.height,
                 properties.border, properties.format, properties.type, properties.pixels);

    if (customProperties.mipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    bool isDepth = properties.internalFormat == GL_DEPTH_COMPONENT;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, isDepth ? GL_NEAREST : customProperties.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, isDepth ? GL_NEAREST : customProperties.magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, isDepth ? GL_CLAMP_TO_BORDER : customProperties.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, isDepth ? GL_CLAMP_TO_BORDER : customProperties.wrapT);
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

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}
