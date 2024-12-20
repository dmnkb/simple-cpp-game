#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Texture
{
    GLuint id;
    int texWidth, texHeight, channelCount;

    void bind(uint32_t slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void unbind(uint32_t slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

class TextureManager
{
  public:
    ~TextureManager();

    static Texture loadTexture(const std::string path);
    static Texture loadTexture(const glm::vec2& dimensions);
    static void deleteTexture(const GLuint& textureID);
    static Ref<Texture> getTextureByID(GLuint textureID);

  private:
    static std::unordered_map<std::string, Ref<Texture>> s_textureCache;
    static std::unordered_map<GLuint, Ref<Texture>> s_textureIDMap;
};