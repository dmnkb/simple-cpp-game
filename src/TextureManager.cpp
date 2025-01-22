#include "TextureManager.h"
#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <glm/glm.hpp>
#include <stb_image.h>

std::unordered_map<std::string, Ref<Texture>> TextureManager::s_textureCache;

TextureManager::~TextureManager()
{
    for (const auto& pair : s_textureCache)
    {
        glDeleteTextures(1, &pair.second->id);
    }
}

Ref<Texture> TextureManager::loadTextureFromFile(const std::string path)
{
    // Return cached texture
    if (s_textureCache.find(path) != s_textureCache.end())
        return s_textureCache[path];

    // Cache miss -> Load texture
    if (!std::filesystem::exists(path))
        fprintf(stderr, "[ERROR] Texture file not found: %s\n", path.c_str());

    int texWidth = 0;
    int texHeight = 0;
    int channelCount = 0;

    unsigned char* data = stbi_load(path.c_str(), &texWidth, &texHeight, &channelCount, 0);

    if (!data)
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        // Could return std::nullopt here or throw. But I'll return empty data for now.
    }

    auto texture = createColorTexture({texWidth, texHeight}, channelCount, data, true);
    s_textureCache[path] = texture;

    stbi_image_free(data);

    return texture;
}

Ref<Texture> TextureManager::createColorTexture(const glm::vec2& dimensions, const int channelCount,
                                                unsigned char* data, bool generateMipmap)
{
    auto texture = CreateRef<Texture>();
    texture->attachmentType = GL_COLOR_ATTACHMENT0;

    GLenum format = GL_RGB;
    if (channelCount == 1)
        format = GL_RED;
    else if (channelCount == 3)
        format = GL_RGB;
    else if (channelCount == 4)
        format = GL_RGBA;

    texture->texWidth = dimensions.x;
    texture->texHeight = dimensions.y;
    texture->channelCount = channelCount;

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexImage2D(GL_TEXTURE_2D, 0, format, dimensions.x, dimensions.y, 0, format, GL_UNSIGNED_BYTE, data);
    if (generateMipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    // Might not be needed for render targets, but keep for now
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

Ref<Texture> TextureManager::createDepthTexture(const glm::vec2& dimensions)
{
    auto depthMap = CreateRef<Texture>();
    depthMap->attachmentType = GL_DEPTH_ATTACHMENT;

    depthMap->texWidth = dimensions.x;
    depthMap->texHeight = dimensions.y;
    depthMap->channelCount = 1;

    glGenTextures(1, &depthMap->id);
    glBindTexture(GL_TEXTURE_2D, depthMap->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, dimensions.x, dimensions.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    return depthMap;
}

void TextureManager::deleteTexture(const GLuint& textureID)
{
    for (const auto& [key, value] : s_textureCache)
    {
        if (value->id == textureID)
        {
            glDeleteTextures(1, &textureID);
            s_textureCache.erase(key);
            return;
        }
    }
    std::cerr << "[ERROR] Can't delete texture ID " << textureID << " as it was not found." << std::endl;
}
