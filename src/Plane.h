#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "Camera.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/glm.hpp>

typedef struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
} Vertex;

static const Vertex vertices[4] = {
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}}, // top left
    {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},  // top right
    {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}}, // bottom right
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}} // bottom left
};

static const GLuint indices[6] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

class Plane
{
  public:
    Plane(const GLuint& textureID, Shader* shader, glm::vec3 position);
    void draw(const Camera& camera);
    void remove();

    GLuint getTexture()
    {
        return m_Texture;
    }

    glm::vec3 getPosition()
    {
        return m_Position;
    }

  private:
    GLuint m_VertexArray;
    GLuint m_VertexBuffer;
    GLuint m_IndexBuffer;
    GLuint m_Texture;
    Shader* m_Shader;
    glm::vec3 m_Position;
};