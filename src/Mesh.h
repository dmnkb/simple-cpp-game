#pragma once

#include "Material.h"
#include "Shader.h"
#include "pch.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

class Mesh
{
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    void bind();
    void unbind();
    GLsizei getIndexCount() const;

  private:
    unsigned int VBO, EBO;

    void setupMesh();
};
