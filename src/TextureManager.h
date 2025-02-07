#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Texture
{
    GLuint id;
    int texWidth, texHeight;
    GLenum format = GL_RGB;
    GLenum type = GL_FLOAT;
    GLenum attachmentType = GL_COLOR_ATTACHMENT0;
    bool mipmaps = true;

    Texture() = default;
    Texture(const std::string& path);

    // Texture framebufferTexture(1024, 1024, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);
    // OR
    // Texture depthTexture(1024, 1024, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
    Texture(const glm::vec2& dimensions, GLenum format, GLenum type, GLenum attachmentType = GL_COLOR_ATTACHMENT0,
            unsigned char* data = NULL, bool generateMipmap = false);

    void create(const glm::vec2& dimensions, GLenum format, GLenum type, GLenum attachmentType = GL_COLOR_ATTACHMENT0,
                unsigned char* data = NULL, bool generateMipmap = false);

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

    ~Texture()
    {
        glDeleteTextures(1, &id);
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