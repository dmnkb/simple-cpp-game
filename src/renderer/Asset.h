#pragma once

#include "core/Core.h"
#include "core/UUID.h"
#include <string>

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
    std::string name = std::string();
    std::filesystem::path path = std::filesystem::path();
    UUID uuid;
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
    UUID importAsset(std::filesystem::path path);
    // serialize/deserialize registry file
  private:
    std::unordered_map<UUID, AssetMetadata> m_meta;
};

class AssetManager
{
  public:
    template <typename T>
    Ref<T> get(UUID id);
    void unload(UUID id);

  private:
    std::unordered_map<UUID, Ref<Asset>> m_loaded;
    Ref<AssetRegistry> m_registry; // to resolve UUID -> path/type
};

} // namespace Engine