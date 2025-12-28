#include "assets/AssetManager.h"
#include "assets/MaterialSerializer.h"
#include "pch.h"

namespace Engine
{

template <typename T>
Ref<T> AssetManager::getOrImport(UUID id)
{
    // 0) basic sanity
    if (id.is_nil()) return nullptr;

    // 1) cache hit
    if (auto it = m_loaded.find(id); it != m_loaded.end()) return std::dynamic_pointer_cast<T>(it->second);

    // 2) registry lookup
    if (!m_registry)
    {
        std::cerr << "AssetManager: No registry set!\n";
        return nullptr;
    }

    const AssetMetadata* meta = m_registry->find(id);
    if (!meta)
    {
        std::cerr << "AssetManager: Asset with UUID " << id.to_string() << " not found in registry!\n";
        return nullptr;
    }

    // 3) type check skipped - AssetTraits not yet implemented
    // TODO: Add type safety with AssetTraits<T>::Type specializations

    // 4) path check
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

    // 5) load via type-specific loader
    Ref<T> asset = AssetLoader::load<T>(*meta);
    if (!asset)
    {
        std::cerr << "AssetManager: Failed to load asset " << id.to_string() << " from '" << meta->path.string()
                  << "'\n";
        return nullptr;
    }

    // 6) ensure metadata on the asset is set (nice to have)
    asset->metadata = *meta;

    // 7) cache and return
    m_loaded[id] = asset;

    std::println("AssetManager: Cached asset '{}' (UUID={})", meta->name, id.to_string());

    return asset;
}

Ref<Material> AssetManager::createMaterial(const std::filesystem::path& path, std::string name)
{
    // 1) register metadata (or reuse if already registered)
    UUID id = m_registry->findOrRegisterAsset(AssetType::Material, path, name);

    // 2) create runtime object
    auto mat = CreateRef<Material>(Shader::getStandardShader());
    mat->metadata = *m_registry->find(id); // or set directly
    mat->metadata.uuid = id;
    mat->metadata.path = path;
    mat->metadata.name = name;

    // 3) cache
    m_loaded[id] = mat;

    // 4) optionally save immediately so it exists on disk
    MaterialSerializer::serialize(mat, path);

    return mat;
}

bool AssetManager::saveAsset(const UUID& id)
{
    if (!m_registry)
    {
        std::cerr << "[AssetManager::saveAsset] No registry set.\n";
        return false;
    }

    const AssetMetadata* meta = m_registry->find(id);
    if (!meta)
    {
        std::cerr << "[AssetManager::saveAsset] Asset not registered. (" << id.to_string() << ")\n";
        return false;
    }

    auto it = m_loaded.find(id);
    if (it == m_loaded.end() || !it->second)
    {
        std::cerr << "[AssetManager::saveAsset] Asset not loaded. (" << id.to_string() << ")\n";
        return false;
    }

    if (meta->type != AssetType::Material)
    {
        std::cerr << "[AssetManager::saveAsset] Can only save materials. (Got: "
                  << AssetTypeNames[static_cast<size_t>(meta->type)] << ")\n";
        return false;
    }

    if (meta->path.empty())
    {
        std::cerr << "[AssetManager::saveAsset] Material has no path set in registry. (" << id.to_string() << ")\n";
        return false;
    }

    Ref<Material> mat = std::dynamic_pointer_cast<Material>(it->second);
    if (!mat)
    {
        std::cerr << "[AssetManager::saveAsset] Loaded asset is not a Material instance. (" << id.to_string() << ")\n";
        return false;
    }

    const bool ok = MaterialSerializer::serialize(mat, meta->path);
    if (!ok)
    {
        std::cerr << "[AssetManager::saveAsset] Failed to serialize material to '" << meta->path.string() << "'\n";
        return false;
    }

    return true;
}

// Explicit template instantiations
template Ref<Material> AssetManager::getOrImport<Material>(UUID id);

} // namespace Engine