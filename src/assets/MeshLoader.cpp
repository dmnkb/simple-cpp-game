#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "assets/MeshLoader.h"
#include "pch.h"
#include "renderer/MaterialManager.h"
#include "renderer/Mesh.h"
#include "renderer/Texture.h"
#include "renderer/TextureManager.h"

namespace Engine
{

std::optional<ModelData> MeshLoader::loadMeshFromFile(std::filesystem::path const& path)
{
    std::println("[Model] Loading model: {}", path.c_str());

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return std::nullopt;
    }

    const std::filesystem::path directory = path.parent_path();

    std::vector<Vertex> allVertices;
    std::vector<unsigned int> allIndices;
    std::vector<Submesh> submeshes;
    std::vector<MeshMaterialData> materials;

    // We'll iterate through all meshes in the scene and consolidate them
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[i];

        Submesh submesh;
        submesh.indexOffset = static_cast<uint32_t>(allIndices.size());
        submesh.indexCount = mesh->mNumFaces * 3;
        submesh.materialIndex = mesh->mMaterialIndex;

        uint32_t vertexOffset = static_cast<uint32_t>(allVertices.size());

        // Vertices
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            Vertex vertex;
            vertex.Position = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};

            if (mesh->HasNormals()) vertex.Normal = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};

            if (mesh->mTextureCoords[0])
                vertex.TexCoords = {mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y};
            else vertex.TexCoords = {0.0f, 0.0f};

            allVertices.push_back(vertex);
        }

        // Indices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                allIndices.push_back(vertexOffset + face.mIndices[j]);
        }

        submeshes.push_back(submesh);
    }

    // Process materials
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* assimpMaterial = scene->mMaterials[i];
        MeshMaterialData materialData;

        auto loadTex = [&](aiTextureType type) -> Ref<Texture>
        {
            auto texs = loadMaterialTextures(assimpMaterial, type, directory);
            return texs.empty() ? nullptr : texs[0];
        };

        materialData.albedo = loadTex(aiTextureType_DIFFUSE);
        materialData.normal = loadTex(aiTextureType_NORMALS);
        materialData.ao = loadTex(aiTextureType_AMBIENT_OCCLUSION);
        materialData.roughness = loadTex(aiTextureType_SHININESS);
        materialData.metallic = loadTex(aiTextureType_METALNESS);

        aiColor3D diffuseColor;
        if (assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
            materialData.baseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);

        materials.push_back(materialData);
    }

    Ref<Mesh> consolidatedMesh = CreateRef<Mesh>(allVertices, allIndices, submeshes);

    return ModelData{consolidatedMesh, materials};
}

// Material texture loading remains the same
std::vector<Ref<Texture>> MeshLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                           std::filesystem::path const& path)
{
    std::vector<Ref<Texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);

        std::string filename = std::string(texturePath.C_Str());
        filename = path.string() + '/' + filename;

        Ref<Texture> texture = CreateRef<Texture>(filename);
        texture->customProperties.materialTextureType = type;
        texture->customProperties.path = texturePath.C_Str();
        textures.push_back(texture);
    }
    return textures;
}

} // namespace Engine