#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <vector>

#include "renderer/Mesh.h"

namespace Engine
{

struct MeshMaterialData
{
    Ref<Texture> albedo;
    Ref<Texture> normal;
    Ref<Texture> roughness;
    Ref<Texture> metallic;
    Ref<Texture> ao;
    glm::vec3 baseColor = glm::vec3(1.0f);
    std::string materialSlotName = std::string();
};

struct ModelData
{
    Ref<Mesh> mesh;
    std::vector<MeshMaterialData> materials;
};

class MeshLoader
{
  public:
    static std::optional<ModelData> loadMeshFromFile(std::filesystem::path const& path);

  private:
    static std::vector<Ref<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                          std::filesystem::path const& path);
};

} // namespace Engine