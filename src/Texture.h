#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <glm/glm.hpp>
#include <stb_image.h>

struct Texture
{
    GLuint id = 0;
    int texWidth = 0, texHeight = 0, channelCount = 0;
    GLenum attachmentType = GL_COLOR_ATTACHMENT0;
    GLenum format = GL_COLOR_COMPONENTS;

    Texture() = default;

    // Texture albedoTexture("assets/textures/albedo.png");
    Texture(const std::string& path)
    {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data)
        {
            std::cerr << "[ERROR] Failed to load texture: " << path << std::endl;
            return;
        }

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        texWidth = width;
        texHeight = height;
        channelCount = channels;
    }

    // Texture framebufferTexture(1024, 1024, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);
    // OR
    // Texture depthTexture(1024, 1024, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
    Texture(int width, int height, GLenum format, GLenum type, GLenum attachment = GL_COLOR_ATTACHMENT0)
        : texWidth(width), texHeight(height), format(format), attachmentType(attachment)
    {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        // Allocate empty texture storage
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, nullptr);

        // Common settings for framebuffers
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void resize(int newWidth, int newHeight, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE)
    {
        texWidth = newWidth;
        texHeight = newHeight;

        glBindTexture(GL_TEXTURE_2D, id);

        // Reallocate memory with new size
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, type, nullptr);

        // Reapply the common settings
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

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
        if (id)
            glDeleteTextures(1, &id);
    }
};
