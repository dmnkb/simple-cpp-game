#include "Model.h"

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#include "Mesh.h"
#include "Texture.h"
#include "pch.h"
#include "renderer/MaterialManager.h"
#include "renderer/ShaderManager.h"
#include "renderer/TextureManager.h"

Model::Model(std::string const& path, const AssetContext& assets, const glm::vec3& position, const glm::vec3& scale)
    : m_position(position), m_scale(scale)
{
    std::println("Loading model: {}", path.c_str());
    loadModel(path, assets);
}

// TODO: Use filesystem path
void Model::loadModel(std::string const& path, const AssetContext& assets)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                       aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, assets);
}

void Model::processNode(aiNode* node, const aiScene* scene, const AssetContext& assets)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto meshData = processMesh(mesh, scene, assets);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), {1.0f, 0.0f, 0.0f}) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), {0.0f, 1.0f, 0.0f}) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.0f), m_scale);

        Renderable renderable = Renderable(meshData.mesh, meshData.material, transform, mesh->mName.C_Str());
        renderables.push_back(renderable);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, assets);
    }
}

const MeshData Model::processMesh(aiMesh* mesh, const aiScene* scene, const AssetContext& assets)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3
            vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly
                    // convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding
    // vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process materials
    aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
    const std::string materialName = assimpMaterial->GetName().C_Str();

    // Material
    Ref<Material> material = assets.materialManager.getMaterialByHandle(materialName);
    if (!material)
    {
        // Shader
        Ref<Shader> shader = assets.shaderManager.getShaderByHandle(materialName);
        if (!shader)
        {
            Ref<Shader> newShader = CreateRef<Shader>("shader/phong.vs", "shader/phong.fs");
            assets.shaderManager.addShader(materialName, newShader);
            shader = newShader;
        }

        // Textures (TODO: Normal maps)
        std::vector<Ref<Texture>> diffuseMaps = loadMaterialTextures(assimpMaterial, aiTextureType_DIFFUSE, assets);
        if (diffuseMaps.empty())
            std::cerr << "Warning: Missing diffuse map (Model::processMesh)" << std::endl;
        Ref<Texture> diffuseMap = diffuseMaps[0];

        material = CreateRef<Material>(shader);
        material->setDiffuseMap(diffuseMap);

        // Important for shadow mapping
        if (materialName == "foliage")
        {
            material->isDoubleSided = true;
        }

        assets.materialManager.addMaterial(materialName, material);
    }

    Ref<Mesh> newMesh = CreateRef<Mesh>(vertices, indices);

    return {.mesh = newMesh, .material = material};
}

std::vector<Ref<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const AssetContext& assets)
{
    std::vector<Ref<Texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);

        std::string filename = std::string(texturePath.C_Str());
        filename = directory + '/' + filename;

        Ref<Texture> texture = assets.textureManager.getTextureByPath(filename);
        texture->customProperties.materialTextureType = type;
        texture->customProperties.path = texturePath.C_Str();
        textures.push_back(texture);
    }
    return textures;
}
