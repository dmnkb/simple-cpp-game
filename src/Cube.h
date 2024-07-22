#pragma once

#define GLAD_GL_IMPLEMENTATION
#include "Camera.h"
#include "Shader.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

static const GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 0
    0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, // B 1
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, // C 2
    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, // D 3
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, // E 4
    0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, // F 5
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // G 6
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, // H 7

    -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, // D 8
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // A 9
    -0.5f, -0.5f, 0.5f,  1.0f, 1.0f, // E 10
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, // H 11
    0.5f,  -0.5f, -0.5f, 0.0f, 0.0f, // B 12
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, // C 13
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // G 14
    0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, // F 15

    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 16
    0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, // B 17
    0.5f,  -0.5f, 0.5f,  1.0f, 1.0f, // F 18
    -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, // E 19
    0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, // C 20
    -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, // D 21
    -0.5f, 0.5f,  0.5f,  1.0f, 1.0f, // H 22
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // G 23
};

// clang-format off
static const GLuint indices[] = {
    // front and back
    0, 3, 2, 2, 1, 0, 4, 5, 6, 6, 7, 4,
    // left and right
    11, 8, 9, 9, 10, 11, 12, 13, 14, 14, 15, 12,
    // bottom and top
    16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
};
// clang-format on

class Cube
{
  public:
    Cube(const GLuint& textureID, Shader* shader, glm::vec3 position);
    void draw(const Camera& camera);
    void remove();
    // clang-format off
    void setPosition(glm::vec3 position) { m_Position = position; }
    void setRotation(glm::vec3 rotation) { m_Rotation = rotation; }
    void setScale(glm::vec3 scale) {  m_Scale = scale; }
    // clang-format on

  private:
    GLuint m_VertexArray, m_VertexBuffer, m_IndexBuffer;
    GLuint m_Texture;
    Shader* m_Shader;

    glm::vec3 m_Position = glm::vec3(0, 0, 0);
    glm::vec3 m_Rotation = glm::vec3(0, 0, 0);
    glm::vec3 m_Scale = glm::vec3(1, 1, 1);
};