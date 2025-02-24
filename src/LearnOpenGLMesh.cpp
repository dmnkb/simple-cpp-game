

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LearnOpenGLMesh.h"
#include "Material.h"
#include "pch.h"

LearnOpenGLMesh::LearnOpenGLMesh(std::vector<LearnOpenGLVertex> vertices, std::vector<unsigned int> indices,
                                 const Ref<Material>& material)
{
    this->vertices = vertices;
    this->indices = indices;
    this->material = material;

    setupMesh();
}

void LearnOpenGLMesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2
    // array which again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LearnOpenGLVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LearnOpenGLVertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LearnOpenGLVertex),
                          (void*)offsetof(LearnOpenGLVertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LearnOpenGLVertex),
                          (void*)offsetof(LearnOpenGLVertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(LearnOpenGLVertex),
                          (void*)offsetof(LearnOpenGLVertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(LearnOpenGLVertex),
                          (void*)offsetof(LearnOpenGLVertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(LearnOpenGLVertex), (void*)offsetof(LearnOpenGLVertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(LearnOpenGLVertex),
                          (void*)offsetof(LearnOpenGLVertex, m_Weights));
    glBindVertexArray(0);
}

void LearnOpenGLMesh::bind()
{
    glBindVertexArray(VAO);
}

void LearnOpenGLMesh::unbind()
{
    glBindVertexArray(0);
}

GLsizei LearnOpenGLMesh::getIndexCount() const
{
    constexpr GLsizei IndexCount = sizeof(indices) / sizeof(indices[0]);
    return IndexCount;
}