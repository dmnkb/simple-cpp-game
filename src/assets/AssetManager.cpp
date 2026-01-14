#include "assets/AssetManager.h"
#include "assets/AssetLoader.h"
#include "assets/MaterialSerializer.h"
#include "pch.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"

namespace Engine
{

static AssetRegistry s_registry;
std::unordered_map<UUID, Ref<Asset>> AssetManager::m_loaded{};

template <typename T>
Ref<T> AssetManager::getOrImport(UUID id)
{
    // TODO: Add type safety with AssetTraits<T>::Type specializations
    // std::println("AssetManager: getOrImport asset type {} UUID={}", typeid(T).name(), id.to_string());

    if (id.is_nil())
    {
        // std::cerr << "AssetManager: Requested asset with nil UUID!\n";
        return nullptr;
    }

    if (auto it = m_loaded.find(id); it != m_loaded.end())
    {
        // std::println("AssetManager: Found cached asset for UUID={}", id.to_string());
        return std::dynamic_pointer_cast<T>(it->second);
    }

    const AssetMetadata* meta = s_registry.find(id);
    if (!meta)
    {
        std::cerr << "AssetManager: Asset with UUID " << id.to_string() << " not found in registry!\n";
        return nullptr;
    }

    if (meta->path.empty())
    {
        std::cerr << "AssetManager: Asset " << id.to_string() << " has empty path in registry!\n";
        return nullptr;
    }

    if (!std::filesystem::exists(meta->path))
    {
        std::cerr << "AssetManager: File not found for asset " << id.to_string() << " at path '" << meta->path.string()
                  << "'\n";
        return nullptr;
    }

    Ref<T> asset = AssetLoader::load<T>(*meta);
    if (!asset)
    {
        std::cerr << "AssetManager: Failed to load asset " << id.to_string() << " from '" << meta->path.string()
                  << "'\n";
        return nullptr;
    }

    std::println("AssetManager: Loaded asset '{}' (UUID={})", meta->name, id.to_string());

    m_loaded[id] = asset;

    std::println("AssetManager: Cached asset '{}' (UUID={})", meta->name, id.to_string());

    return asset;
}

Ref<Material> AssetManager::createMaterial(const std::filesystem::path& path, std::string name)
{
    std::println("AssetManager: Creating new material '{}'", name);
    UUID id = s_registry.findOrRegisterAsset(AssetType::Material, path, name);

    auto mat = CreateRef<Material>();

    mat->shader = Shader::getStandardShader();
    mat->metadata.uuid = id;
    mat->metadata.path = path;
    mat->metadata.name = name;

    m_loaded[id] = mat;

    // TODO: Decide when to serialize - on creation or on first save
    // MaterialSerializer::serialize(mat, path);

    return mat;
}

// bool AssetManager::saveAsset(const UUID& id)
// {
//     assert(!m_registry && "AssetManager::saveAsset called without registry set");

//     const AssetMetadata* meta = m_registry->find(id);
//     if (!meta)
//     {
//         std::cerr << "[AssetManager::saveAsset] Asset not registered. (" << id.to_string() << ")\n";
//         return false;
//     }

//     auto it = m_loaded.find(id);
//     if (it == m_loaded.end() || !it->second)
//     {
//         std::cerr << "[AssetManager::saveAsset] Asset not loaded. (" << id.to_string() << ")\n";
//         return false;
//     }

//     if (meta->type != AssetType::Material)
//     {
//         std::cerr << "[AssetManager::saveAsset] Can only save materials. (Got: "
//                   << AssetTypeNames[static_cast<size_t>(meta->type)] << ")\n";
//         return false;
//     }

//     if (meta->path.empty())
//     {
//         std::cerr << "[AssetManager::saveAsset] Material has no path set in registry. (" << id.to_string() << ")\n";
//         return false;
//     }

//     Ref<Material> mat = std::dynamic_pointer_cast<Material>(it->second);
//     if (!mat)
//     {
//         std::cerr << "[AssetManager::saveAsset] Loaded asset is not a Material instance. (" << id.to_string() <<
//         ")\n"; return false;
//     }

//     const bool ok = MaterialSerializer::serialize(mat, meta->path);
//     if (!ok)
//     {
//         std::cerr << "[AssetManager::saveAsset] Failed to serialize material to '" << meta->path.string() << "'\n";
//         return false;
//     }

//     return true;
// }

// Explicit template instantiations
template Ref<Material> AssetManager::getOrImport<Material>(UUID id);
template Ref<Mesh> AssetManager::getOrImport<Mesh>(UUID id);
template Ref<Shader> AssetManager::getOrImport<Shader>(UUID id);
template Ref<Texture> AssetManager::getOrImport<Texture>(UUID id);

} // namespace Engine