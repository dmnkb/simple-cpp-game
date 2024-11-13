#pragma once

#include "pch.h"
#include <glad/glad.h>

struct Texture
{
    GLuint id;
    int texWidth, texHeight, channelCount;

    void bind(uint32_t slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }
};

class TextureManager
{
  public:
    ~TextureManager();

    static Texture loadTexture(const std::string path);
    static void deleteTexture(const GLuint& textureID);
    static std::shared_ptr<Texture> getTextureByID(GLuint textureID);

  private:
    static std::unordered_map<std::string, std::shared_ptr<Texture>> s_textureCache;
    static std::unordered_map<GLuint, std::shared_ptr<Texture>> s_textureIDMap;
};