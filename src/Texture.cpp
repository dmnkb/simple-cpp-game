#include "Texture.h"
#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <glm/glm.hpp>
#include <stb_image.h>

Texture::Texture(const std::string& path)
{
    if (!std::filesystem::exists(path))
        fprintf(stderr, "[ERROR] Texture file not found: %s\n", path.c_str());

    int channelCount;
    unsigned char* data = stbi_load(path.c_str(), &texWidth, &texHeight, &channelCount, 0);

    if (!data)
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        // Could return std::nullopt here or throw. But I'll return empty data for now.
    }

    GLenum format = GL_RGB;
    if (channelCount == 1)
        format = GL_RED;
    else if (channelCount == 3)
        format = GL_RGB;
    else if (channelCount == 4)
        format = GL_RGBA;

    create({texWidth, texHeight}, format, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, data, true);

    stbi_image_free(data);
}

Texture::Texture(const glm::vec2& dimensions, GLenum format, GLenum type, GLenum attachmentType, unsigned char* data,
                 bool generateMipmap)
    : attachmentType(attachmentType)
{
    create(dimensions, format, type, attachmentType, data);
}

void Texture::create(const glm::vec2& dimensions, GLenum format, GLenum type, GLenum attachmentType,
                     unsigned char* data, bool generateMipmap)
{
    texWidth = dimensions.x;
    texHeight = dimensions.y;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, format, dimensions.x, dimensions.y, 0, format, type, data);

    if (generateMipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    if (format == GL_DEPTH_COMPONENT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}