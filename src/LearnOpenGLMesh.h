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
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
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
