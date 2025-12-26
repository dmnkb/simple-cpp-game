#pragma once

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

#include "assets/AssetLoader.h"
#include "core/Core.h"
#include "core/UUID.h"

namespace Engine
{

enum class AssetType
{
    None = 0,
    Mesh,
    Texture,
    Shader,
    Material,
    Scene,
};

struct AssetMetadata
{
    AssetType type = AssetType::None;
    std::string name = "Unnamed Asset";
    std::filesystem::path path = "Unknown Path";
    UUID uuid = UUID::random();
};

class Asset
{
  public:
    virtual ~Asset() = default;
    AssetMetadata metadata;
};

class AssetRegistry
{
  public:
    const AssetMetadata* find(UUID id) const;

    UUID findOrRegisterAsset(AssetType type, const std::filesystem::path path = std::filesystem::path(),
                             const std::string& name = std::string());

    template <typename Fn>
    void forEachByType(AssetType type, Fn&& fn) const
    {
        for (auto& [id, meta] : m_meta)
            if (meta.type == type) fn(id, meta);
    }

    // serialize/deserialize registry file
  private:
    std::unordered_map<UUID, AssetMetadata> m_meta;

    std::optional<UUID> findAsset(const std::filesystem::path path) const;
};

class AssetManager
{
  public:
    AssetManager(const Ref<AssetRegistry>& registry) : m_registry(registry) {}
    ~AssetManager() = default;

    template <typename T>
    Ref<T> getOrImport(UUID id)
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
            std::cerr << "AssetManager: File not found for asset " << id.to_string() << " at path '"
                      << meta->path.string() << "'\n";
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

    template <typename T>
    void unload(UUID id);

  private:
    std::unordered_map<UUID, Ref<Asset>> m_loaded;
    Ref<AssetRegistry> m_registry; // to resolve UUID -> path/type
};

} // namespace Engine