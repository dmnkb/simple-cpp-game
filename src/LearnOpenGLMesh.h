#pragma once

#include "Material.h"
#include "Shader.h"
#include "pch.h"

#define MAX_BONE_INFLUENCE 4

struct LearnOpenGLVertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

class LearnOpenGLMesh
{
  public:
    std::vector<LearnOpenGLVertex> vertices;
    std::vector<unsigned int> indices;
    Ref<Material> material;

    unsigned int VAO;

    LearnOpenGLMesh(std::vector<LearnOpenGLVertex> vertices, std::vector<unsigned int> indices,
                    const Ref<Material>& material);

    void bind();
    void unbind();
    GLsizei getIndexCount() const;

  private:
    unsigned int VBO, EBO;

    void setupMesh();
};
