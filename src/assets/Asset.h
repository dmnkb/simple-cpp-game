#pragma once

#include <filesystem>
#include <string>

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

// Stores “what assets exist” and their IDs/paths/types. This survives restarts.
class AssetRegistry
{
  public:
    const AssetMetadata* find(UUID id) const
    {
        auto it = m_meta.find(id);
        if (it != m_meta.end()) return &it->second;
        return nullptr;
    }

    UUID importAsset(std::filesystem::path path);

    template <typename Fn>
    void forEachByType(AssetType type, Fn&& fn) const
    {
        for (auto& [id, meta] : m_meta)
            if (meta.type == type) fn(id, meta);
    }

    // serialize/deserialize registry file
  private:
    std::unordered_map<UUID, AssetMetadata> m_meta;
};

// Stores “what assets are currently loaded”
class AssetManager
{
  public:
    AssetManager(const Ref<AssetRegistry>& registry) : m_registry(registry) {}
    ~AssetManager() = default;

    template <typename T>
    Ref<T> getOrImport(UUID id);

    void addLoaded(UUID id, const Ref<Asset>& asset) { m_loaded[id] = asset; }

    template <typename T>
    void unload(UUID id);

  private:
    std::unordered_map<UUID, Ref<Asset>> m_loaded;
    Ref<AssetRegistry> m_registry; // to resolve UUID -> path/type
};

} // namespace Engine