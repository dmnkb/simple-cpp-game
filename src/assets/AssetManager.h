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
    // Project root for resolving relative asset paths
    static std::filesystem::path getProjectRoot()
    {
        static std::filesystem::path s_projectRoot = []()
        {
            auto cwd = std::filesystem::current_path();

            // If running from build directory, go up one level
            if (cwd.filename() == "build" && std::filesystem::exists(cwd.parent_path() / "assets"))
            {
                return cwd.parent_path();
            }

            // Otherwise assume we're already at project root
            return cwd;
        }();
        return s_projectRoot;
    }

    // Convert relative path to absolute
    static std::filesystem::path toAbsolutePath(const std::filesystem::path& relativePath)
    {
        if (relativePath.is_absolute()) return relativePath;
        return getProjectRoot() / relativePath;
    }

    // Convert absolute path to relative (relative to project root)
    static std::filesystem::path toRelativePath(const std::filesystem::path& absolutePath)
    {
        if (absolutePath.is_relative()) return absolutePath;
        return std::filesystem::relative(absolutePath, getProjectRoot());
    }

    template <typename T>
    static Ref<T> getOrImport(UUID id);

    static Ref<Material> createMaterial(const std::filesystem::path& path, std::string name);

    // TODO: consider putting somewhere else?
    static Ref<Material> getDefaultMaterial()
    {
        static Ref<Material> defaultMaterial;
        if (!defaultMaterial)
        {
            const auto defaultMaterialId = AssetRegistry::findOrRegisterAsset(
                AssetType::Material, "assets/materials/default.mat", "Default Material");

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