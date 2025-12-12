#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <vector>

#include "renderer/Mesh.h"

namespace Engine
{

struct LoadedSubmesh
{
    Ref<Mesh> mesh;
    struct
    {
        Ref<Texture> albedo;
        Ref<Texture> normal;
        Ref<Texture> roughness;
        Ref<Texture> metallic;
        Ref<Texture> ao;
        glm::vec3 baseColor = glm::vec3(1.0f);
    } materialData;
};

class MeshLoader
{
  public:
    static std::optional<std::vector<LoadedSubmesh>> loadMeshFromFile(std::filesystem::path const& path);

  private:
    static std::vector<LoadedSubmesh> processNode(aiNode* node, const aiScene* scene,
                                                  std::filesystem::path const& path);
    static LoadedSubmesh processMesh(aiMesh* mesh, const aiScene* scene, std::filesystem::path const& directory);
    static std::vector<Ref<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                          std::filesystem::path const& path);
};

} // namespace Engine