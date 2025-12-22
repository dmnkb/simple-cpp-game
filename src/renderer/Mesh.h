#pragma once

#include "renderer/Asset.h"
#include "renderer/Material.h"
#include "renderer/Shader.h"

namespace Engine
{

// TODO:
#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Submesh
{
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t materialIndex;
};

class Mesh : public Asset
{
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Submesh> submeshes;

    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Submesh> submeshes);

    void bind();
    void unbind();
    GLsizei getIndexCount() const;

  private:
    unsigned int VBO, EBO;

    void setupMesh();
};

} // namespace Engine