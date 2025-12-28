#pragma once

#include "assets/Asset.h"
#include "assets/AssetRegistry.h"
#include "core/Core.h"
#include "core/UUID.h"

namespace Engine
{

class AssetManager
{
  public:
    AssetManager(const Ref<AssetRegistry>& registry) : m_registry(registry) {}
    ~AssetManager() = default;

    template <typename T>
    Ref<T> getOrImport(UUID id);

    Ref<Material> createMaterial(const std::filesystem::path& path, std::string name);

    bool saveAsset(const UUID& id);

    // template <typename T>
    // void unload(UUID id);

  private:
    std::unordered_map<UUID, Ref<Asset>> m_loaded;
    Ref<AssetRegistry> m_registry;
};

} // namespace Engine