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

#include "LearnOpenGLMesh.h"
#include "Texture.h"
#include "pch.h"

Model::Model(std::string const& path)
{
    loadModel(path);
}

void Model::loadModel(std::string const& path)
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
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        auto processedMesh = processMesh(mesh, scene);
        meshes.push_back(processedMesh);

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), {1.0f, 0.0f, 0.0f}) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), {0.0f, 1.0f, 0.0f}) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), {0.0f, 0.0f, 1.0f}) *
                              glm::scale(glm::mat4(1.0f), m_scale);

        const auto renderable = CreateRef<Renderable>(processedMesh, processedMesh->material, transform);
        renderables.push_back(renderable);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Ref<LearnOpenGLMesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<LearnOpenGLVertex> vertices;
    std::vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        LearnOpenGLVertex vertex;
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
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding
    // vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    const std::string materialName = material->GetName().C_Str();

    Ref<Material> mappedMaterial = nullptr;

    switch (str2int(materialName.c_str()))
    {
    default:
        auto shader = CreateRef<Shader>("assets/phong.vs", "assets/phong.fs");
        mappedMaterial = CreateRef<Material>(shader);
        mappedMaterial->name = materialName;

        break;
    }

    // 1. diffuse maps
    auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    mappedMaterial->setDiffuseMap(diffuseMaps[0]); // TODO: Support more loaded textures

    // return a mesh object created from the extracted mesh data
    return CreateRef<LearnOpenGLMesh>(vertices, indices, mappedMaterial);
}

std::vector<Ref<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    std::vector<Ref<Texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j]->path.data(), texturePath.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one.
                             // (optimization)
                break;
            }
        }
        if (!skip)
        { // if texture hasn't been loaded already, load it
            std::string filename = std::string(texturePath.C_Str());
            filename = directory + '/' + filename;
            auto texture = CreateRef<Texture>(filename);
            texture->materialTextureType = type;
            texture->path = texturePath.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't
                                                // unnecessary load duplicate textures.
        }
    }
    return textures;
}
