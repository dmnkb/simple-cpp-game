#pragma once

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

#include "assets/Asset.h"
#include "core/UUID.h"

namespace Engine
{

class AssetRegistry
{
  public:
    static const AssetMetadata* find(UUID id);

    static UUID findOrRegisterAsset(AssetType type, const std::filesystem::path path = std::filesystem::path(),
                                    const std::string& name = std::string());

    static void unregisterAssetByID(UUID id);

    template <typename Fn>
        requires std::invocable<Fn, AssetType, std::string_view>
    static void forEachAssetType(Fn&& fn)
    {
        for (int i = static_cast<int>(AssetType::Mesh); i < static_cast<int>(AssetType::Size); ++i)
        {
            fn(static_cast<AssetType>(i), AssetTypeNames[i]);
        }
    }

    template <typename Fn>
    static void forEachByType(AssetType type, Fn&& fn)
    {
        for (auto& [id, meta] : m_meta)
            if (meta.type == type) fn(id, meta);
    }

    static std::unordered_map<UUID, AssetMetadata> getAllAssetsByType(AssetType type)
    {
        std::unordered_map<UUID, AssetMetadata> result;
        for (const auto& [id, meta] : m_meta)
        {
            if (meta.type == type) result[id] = meta;
        }
        return result;
    }

    // TODO: serialize/deserialize registry file
  private:
    inline static std::unordered_map<UUID, AssetMetadata> m_meta;

  private:
    static std::optional<UUID> findAsset(const std::filesystem::path path);
};

} // namespace Engine