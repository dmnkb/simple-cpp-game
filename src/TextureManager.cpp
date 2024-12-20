#include "TextureManager.h"
#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <glm/glm.hpp>
#include <stb_image.h>

std::unordered_map<std::string, Ref<Texture>> TextureManager::s_textureCache;
std::unordered_map<GLuint, Ref<Texture>> TextureManager::s_textureIDMap;

TextureManager::~TextureManager()
{
    for (const auto& pair : s_textureCache)
    {
        glDeleteTextures(1, &pair.second->id);
    }
}

Texture TextureManager::loadTexture(const std::string path)
{
    // Return cached texture
    if (s_textureCache.find(path) != s_textureCache.end())
        return *s_textureCache[path];

    // Load texture
    if (!std::filesystem::exists(path))
    {
        fprintf(stderr, "[ERROR] Texture file not found: %s\n", path.c_str());
    }

    Texture texture;

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load(path.c_str(), &texture.texWidth, &texture.texHeight, &texture.channelCount, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (texture.channelCount == 1)
            format = GL_RED;
        else if (texture.channelCount == 3)
            format = GL_RGB;
        else if (texture.channelCount == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.texWidth, texture.texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    auto sharedTexture = CreateRef<Texture>(texture);
    s_textureCache[path] = sharedTexture;
    s_textureIDMap[texture.id] = sharedTexture;

    return texture;
}

Texture TextureManager::loadTexture(const glm::vec2& dimensions)
{
    Texture texture;

    texture.texWidth = dimensions.x;
    texture.texHeight = dimensions.y;
    texture.channelCount = 3;

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dimensions.x, dimensions.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
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
    std::cerr << "[ERROR] Can't delete texture ID " << textureID << "as it was not found." << std::endl;
}

Ref<Texture> TextureManager::getTextureByID(GLuint textureID)
{
    auto it = s_textureIDMap.find(textureID);
    if (it != s_textureIDMap.end())
    {
        return it->second;
    }

    std::cerr << "[ERROR] Can't resolve texture ID " << textureID << std::endl;
    return nullptr;
}