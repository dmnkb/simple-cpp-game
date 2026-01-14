#pragma once

#include "assets/Asset.h"
#include "assets/AssetRegistry.h"
#include "core/Core.h"
#include "core/UUID.h"
#include "renderer/Material.h"

namespace Engine
{

class AssetManager
{
  public:
    template <typename T>
    static Ref<T> getOrImport(UUID id);

    static Ref<Material> createMaterial(const std::filesystem::path& path, std::string name);
    // TODO: consider putting somewhere else?
    static Ref<Material> getDefaultMaterial()
    {
        static Ref<Material> defaultMaterial;
        if (!defaultMaterial)
        {
            // FIXME: Hardcoded path
            const auto defaultMaterialId = AssetRegistry::findOrRegisterAsset(
                AssetType::Material, "/Users/dominikborchert/Desktop/simple-cpp-game/assets/materials/default.mat",
                "Default Material");

            defaultMaterial = AssetManager::getOrImport<Material>(defaultMaterialId);
        }
        return defaultMaterial;
    }

    // bool saveAsset(const UUID& id);

    // template <typename T>
    // void unload(UUID id);

  private:
    static std::unordered_map<UUID, Ref<Asset>> m_loaded;
};

} // namespace Engine