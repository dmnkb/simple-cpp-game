#pragma once

#include "pch.h"
#include <glad/glad.h>

typedef struct
{
    GLuint id;
    int texWidth, texHeight, nrChannels;
} Texture;

class TextureManager
{
  public:
    ~TextureManager();

    Texture loadTexture(const std::string path);
    void deleteTexture(const GLuint& textureID);

  private:
    std::unordered_map<std::string, Texture> m_textureCache;
};