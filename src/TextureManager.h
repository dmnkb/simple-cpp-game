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

    static Texture loadTexture(const std::string path);
    static void deleteTexture(const GLuint& textureID);

  private:
    static std::unordered_map<std::string, Texture> s_textureCache;
};