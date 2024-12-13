#pragma once

#include "pch.h"
#include <glad/glad.h>

static const GLfloat vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 0
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // B 1
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // C 2
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // D 3

    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // E 4
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // F 5
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // G 6
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // H 7

    // Left face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 8
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f,  // D 9
    -0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // H 10
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,  // E 11

    // Right face
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // B 12
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f,  // C 13
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // G 14
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f,  // F 15

    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 16
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // B 17
    0.5f, -0.5f, 0.5f, 1.0f, 1.0f,   // F 18
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,  // E 19

    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, // D 20
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f,  // C 21
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // G 22
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f   // H 23
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

// TODO: use!
static const GLfloat normals[] = {
    // Back face normals
    0.0f, 0.0f, -1.0f, // A 0
    0.0f, 0.0f, -1.0f, // B 1
    0.0f, 0.0f, -1.0f, // C 2
    0.0f, 0.0f, -1.0f, // D 3

    // Front face normals
    0.0f, 0.0f, 1.0f, // E 4
    0.0f, 0.0f, 1.0f, // F 5
    0.0f, 0.0f, 1.0f, // G 6
    0.0f, 0.0f, 1.0f, // H 7

    // Left face normals
    -1.0f, 0.0f, 0.0f, // A 8
    -1.0f, 0.0f, 0.0f, // D 9
    -1.0f, 0.0f, 0.0f, // H 10
    -1.0f, 0.0f, 0.0f, // E 11

    // Right face normals
    1.0f, 0.0f, 0.0f, // B 12
    1.0f, 0.0f, 0.0f, // C 13
    1.0f, 0.0f, 0.0f, // G 14
    1.0f, 0.0f, 0.0f, // F 15

    // Bottom face normals
    0.0f, -1.0f, 0.0f, // A 16
    0.0f, -1.0f, 0.0f, // B 17
    0.0f, -1.0f, 0.0f, // F 18
    0.0f, -1.0f, 0.0f, // E 19

    // Top face normals
    0.0f, 1.0f, 0.0f, // D 20
    0.0f, 1.0f, 0.0f, // C 21
    0.0f, 1.0f, 0.0f, // G 22
    0.0f, 1.0f, 0.0f, // H 23
};

class Mesh
{
  public:
    Mesh(const std::string& modelPath = "");
    ~Mesh();

    void bind(GLuint instanceBuffer = 0);
    void unbind();
    GLsizei getIndexCount() const;

  private:
    GLuint m_VertexArray, m_VertexBuffer, m_IndexBuffer;
};