#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Texture
{
    GLuint id;
    int texWidth, texHeight, channelCount;
    GLenum attachmentType;

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

    static Ref<Texture> loadTextureFromFile(const std::string path);
    static Ref<Texture> createColorTexture(const glm::vec2& dimensions, const int channelCount = 3,
                                           unsigned char* data = NULL, bool generateMipmap = false);
    static Ref<Texture> createDepthTexture(const glm::vec2& dimensions);
    static void deleteTexture(const GLuint& textureID);

  private:
    static std::unordered_map<std::string, Ref<Texture>> s_textureCache;
};