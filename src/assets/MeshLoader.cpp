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

std::optional<std::vector<LoadedSubmesh>> MeshLoader::loadMeshFromFile(std::filesystem::path const& path)
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

    return processNode(scene->mRootNode, scene, directory);
}

std::vector<LoadedSubmesh> MeshLoader::processNode(aiNode* node, const aiScene* scene,
                                                   std::filesystem::path const& path)
{
    std::vector<LoadedSubmesh> subMeshes;

    // Process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        LoadedSubmesh meshData = processMesh(mesh, scene, path);

        subMeshes.push_back(meshData);
    }

    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        std::vector<LoadedSubmesh> childSubMeshes = processNode(node->mChildren[i], scene, path);
        subMeshes.insert(subMeshes.end(), childSubMeshes.begin(), childSubMeshes.end());
    }

    return subMeshes;
}

LoadedSubmesh MeshLoader::processMesh(aiMesh* mesh, const aiScene* scene, std::filesystem::path const& directory)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        // Positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // Normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        // Texture coordinates
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // Vertex indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Material
    aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Ref<Texture>> diffuseMaps = loadMaterialTextures(assimpMaterial, aiTextureType_DIFFUSE, directory);
    Ref<Texture> diffuseMap;
    if (!diffuseMaps.empty())
        diffuseMap = diffuseMaps[0];

    std::vector<Ref<Texture>> normalMaps = loadMaterialTextures(assimpMaterial, aiTextureType_NORMALS, directory);
    Ref<Texture> normalMap = nullptr;
    if (!normalMaps.empty())
        normalMap = normalMaps[0];

    std::vector<Ref<Texture>> specularMaps = loadMaterialTextures(assimpMaterial, aiTextureType_SPECULAR, directory);
    Ref<Texture> specularMap = nullptr;
    if (!specularMaps.empty())
        specularMap = specularMaps[0];

    std::vector<Ref<Texture>> aoMaps = loadMaterialTextures(assimpMaterial, aiTextureType_AMBIENT_OCCLUSION, directory);
    Ref<Texture> aoMap = nullptr;
    if (!aoMaps.empty())
        aoMap = aoMaps[0];

    std::vector<Ref<Texture>> roughnessMaps = loadMaterialTextures(assimpMaterial, aiTextureType_SHININESS, directory);
    Ref<Texture> roughnessMap = nullptr;
    if (!roughnessMaps.empty())
        roughnessMap = roughnessMaps[0];

    std::vector<Ref<Texture>> metallicMaps = loadMaterialTextures(assimpMaterial, aiTextureType_METALNESS, directory);
    Ref<Texture> metallicMap = nullptr;
    if (!metallicMaps.empty())
        metallicMap = metallicMaps[0];

    // Extract material properties from Assimp
    glm::vec3 baseColor(1.0f);
    aiColor3D diffuseColor;
    if (assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
        baseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);

    float shininess = 32.0f;
    assimpMaterial->Get(AI_MATKEY_SHININESS, shininess);

    Ref<Mesh> newMesh = CreateRef<Mesh>(vertices, indices);

    return {newMesh,
            {.albedo = diffuseMap,
             .normal = normalMap,
             .roughness = roughnessMap,
             .metallic = metallicMap,
             .ao = aoMap,
             .baseColor = baseColor}};
}

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