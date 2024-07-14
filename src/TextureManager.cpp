#include "TextureManager.h"
#include <filesystem>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

TextureManager::~TextureManager()
{
    for (const auto& pair : m_textureCache)
    {
        glDeleteTextures(1, &pair.second.id);
    }
}

Texture TextureManager::loadTexture(const std::string path)
{
    printf("Loading texture: %s\n", path.c_str());
    Texture texture;

    // Return cached texture
    if (m_textureCache.find(path) != m_textureCache.end())
    {
        return m_textureCache[path];
    }

    // Load texture
    if (!std::filesystem::exists(path))
    {
        fprintf(stderr, "[ERROR] Texture file not found: %s\n", path.c_str());
    }

    unsigned char* data = stbi_load(path.c_str(), &texture.texWidth, &texture.texHeight, &texture.nrChannels, 0);
    if (!data)
    {
        fprintf(stderr, "[ERROR] Failed to load texture\n");
    }

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format;
    if (texture.nrChannels == 1)
        format = GL_RED;
    else if (texture.nrChannels == 3)
        format = GL_RGB;
    else if (texture.nrChannels == 4)
        format = GL_RGBA;
    else
    {
        std::cerr << "[ERROR] Unsupported number of channels: " << texture.nrChannels << std::endl;
        stbi_image_free(data);
        return texture;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.texWidth, texture.texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    m_textureCache[path] = texture;
    return texture;
}

void TextureManager::deleteTexture(const GLuint& textureID)
{
    for (const auto& [key, value] : m_textureCache)
    {
        if (value.id == textureID)
        {
            glDeleteTextures(1, &textureID);
            m_textureCache.erase(key);
            return;
        }
    }
    std::cerr << "[ERROR] Can't delete texture ID " << textureID << "as it was not found." << std::endl;
}