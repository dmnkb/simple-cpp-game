#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "pch.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// An atomic, stateless POD (plain old data) that contains mesh, material and transform.
struct Renderable
{
    Ref<Mesh> mesh;
    Ref<Material> material;
    glm::mat4 transform;
    std::string name;
};

class Model
{
  public:
    // model data
    std::vector<Ref<Texture>> textures_loaded; // stores all the textures loaded so far, optimization to make sure
                                               // textures aren't loaded more than once.
    // FIXME: Should be unique
    std::vector<Ref<Renderable>> renderables;
    std::string directory;

    glm::vec3 m_position, m_rotation = glm::vec3{0.0f};
    glm::vec3 m_scale = glm::vec3{1.0f};

    Model(std::string const& path, const glm::vec3& position = {0.0, 0.0, 0.0},
          const glm::vec3& scale = {1.0, 1.0, 1.0});

  private:
    void loadModel(std::string const& path);

    void processNode(aiNode* node, const aiScene* scene);

    Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Ref<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    constexpr unsigned int str2int(const char* str, int h = 0)
    {
        return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
    }

  private:
    Ref<Material> m_Material; // TODO: remove
};
