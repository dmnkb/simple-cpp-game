#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 UV;
    glm::vec3 Normal;
};

static const Vertex vertices[] = {
    // Position             UV          Normal
    // Back face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // 0
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // 1
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},   // 2
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},  // 3

    // Front face
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 4
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // 5
    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},   // 6
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // 7

    // Left face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, // 8
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},  // 9
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},   // 10
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},  // 11

    // Right face
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 12
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // 13
    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},   // 14
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},  // 15

    // Bottom face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}, // 16
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},  // 17
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},   // 18
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},  // 19

    // Top face
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 20
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // 21
    {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},   // 22
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},  // 23
};

static const uint32_t indices[] = {
    2,  1,  0,  0,  3,  2,  // Back face
    4,  5,  6,  6,  7,  4,  // Front face
    10, 9,  8,  8,  11, 10, // Left face
    12, 13, 14, 14, 15, 12, // Right face
    16, 17, 18, 18, 19, 16, // Bottom face
    22, 21, 20, 20, 23, 22, // Top face
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