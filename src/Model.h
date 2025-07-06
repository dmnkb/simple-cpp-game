#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "pch.h"
#include "renderer/MaterialManager.h"
#include "renderer/ShaderManager.h"
#include "renderer/TextureManager.h"
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
struct MeshData
{
    Ref<Mesh> mesh;
    Ref<Material> material;
};

struct AssetContext
{
    MaterialManager& materialManager;
    ShaderManager& shaderManager;
    TextureManager& textureManager;

    AssetContext(MaterialManager& matMgr, ShaderManager& shdMgr, TextureManager& texMgr)
        : materialManager(matMgr), shaderManager(shdMgr), textureManager(texMgr)
    {
    }
};

class Model
{
  public:
    std::vector<Renderable> renderables;
    std::string directory;

    glm::vec3 m_position, m_rotation = glm::vec3{0.0f};
    glm::vec3 m_scale = glm::vec3{1.0f};

    Model(std::string const& path, const AssetContext& assets, const glm::vec3& position = {0.0, 0.0, 0.0},
          const glm::vec3& scale = {1.0, 1.0, 1.0});

  private:
    void loadModel(std::string const& path, const AssetContext& assets);
    void processNode(aiNode* node, const aiScene* scene, const AssetContext& assets);
    const MeshData processMesh(aiMesh* mesh, const aiScene* scene, const AssetContext& assets);
    std::vector<Ref<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const AssetContext& assets);
};
