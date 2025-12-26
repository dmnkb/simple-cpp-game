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
    const AssetMetadata* find(UUID id) const;
    UUID importAsset(std::filesystem::path path);
    // serialize/deserialize registry file
  private:
    std::unordered_map<UUID, AssetMetadata> m_meta;
};

// Stores “what assets are currently loaded”
class AssetManager
{
  public:
    template <typename T>
    Ref<T> getOrImport(UUID id);
    template <typename T>
    void unload(UUID id);

  private:
    std::unordered_map<UUID, Ref<Asset>> m_loaded;
    Ref<AssetRegistry> m_registry; // to resolve UUID -> path/type
};

} // namespace Engine