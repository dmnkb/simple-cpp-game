
#include "pch.h"
#include <fmt/format.h>

#include "assets/Asset.h"
#include "assets/AssetLoader.h"
#include "assets/MaterialSerializer.h"
#include "assets/MeshLoader.h"
#include "renderer/Material.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "scene/Scene.h"

namespace Engine
{

template <typename T>
static bool isSane(const AssetMetadata& meta)
{
    if (meta.path.empty())
    {
        std::cerr << fmt::format("AssetLoader<{}>: empty path (uuid={})\n", typeid(T).name(), meta.uuid.to_string());
        return false;
    }

    if (!std::filesystem::exists(meta.path))
    {
        std::cerr << fmt::format("AssetLoader<{}>: file not found '{}' (uuid={})\n", typeid(T).name(),
                                 meta.path.string(), meta.uuid.to_string());
        return false;
    }

    return true;
}

// MARK: Texture
template <>
Ref<Texture> AssetLoader::load<Texture>(const AssetMetadata& meta)
{
    if (!isSane<Texture>(meta)) return nullptr;

    Ref<Texture> texture = CreateRef<Texture>(meta.path.string());
    texture->metadata = meta;
    return texture;
}

// MARK: Shader
template <>
Ref<Shader> AssetLoader::load<Shader>(const AssetMetadata& meta)
{
    if (!isSane<Shader>(meta)) return nullptr;

    // FIXME: What to do about separate vertex/fragment paths?
    // Currently passing the same path for both - needs proper shader asset format
    std::string pathStr = meta.path.string();
    Ref<Shader> shader = CreateRef<Shader>(pathStr.c_str(), pathStr.c_str());
    shader->metadata = meta;
    return shader;
}

// MARK: Material
template <>
Ref<Material> AssetLoader::load<Material>(const AssetMetadata& meta)
{
    if (!isSane<Material>(meta)) return nullptr;

    std::println("AssetLoader<Material>: Loading material '{}' from '{}'", meta.name, meta.path.string());

    Ref<Material> material = CreateRef<Material>();
    MaterialSerializer::deserialize(material, meta.path);

    return material;
}

// MARK: Mesh
template <>
Ref<Mesh> AssetLoader::load<Mesh>(const AssetMetadata& meta)
{
    if (!isSane<Mesh>(meta)) return nullptr;

    const auto modelDataOpt = MeshLoader::loadMeshFromFile(meta.path.string());
    if (!modelDataOpt)
    {
        std::cerr << fmt::format("AssetLoader<Mesh>: Failed to load model data from '{}' (uuid={})\n",
                                 meta.path.string(), meta.uuid.to_string());
        return nullptr;
    }

    const auto resolvedModelData = *modelDataOpt;

    Ref<Mesh> mesh = resolvedModelData.mesh;
    mesh->metadata = meta;

    for (size_t i = 0; i < resolvedModelData.materials.size(); ++i)
    {
        const auto& matData = resolvedModelData.materials[i];

        std::println("AssetLoader<Mesh>: Loaded material slot '{}' for mesh '{}'", matData.materialSlotName, meta.name);

        // Create a default material for this slot
        Ref<Material> material = AssetManager::createMaterial(
            fmt::format("/Users/dominikborchert/Desktop/simple-cpp-game/assets/materials/{}_{}.mat", meta.name, i),
            fmt::format("{}_Material_{}", meta.name, i));

        material->albedo = matData.albedo;
        material->normal = matData.normal;
        material->roughness = matData.roughness;
        material->metallic = matData.metallic;
        material->ao = matData.ao;
        material->baseColor = matData.baseColor;
        // TODO: Slot name and base color?

        mesh->defaultMaterialSlots.push_back(material);
    }

    return mesh;
}

template <>
Ref<Scene> AssetLoader::load<Scene>(const AssetMetadata& meta)
{
    assert(false && "AssetLoader::load<Scene> not implemented yet");
}

} // namespace Engine
