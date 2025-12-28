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
    const AssetMetadata* find(UUID id) const;

    UUID findOrRegisterAsset(AssetType type, const std::filesystem::path path = std::filesystem::path(),
                             const std::string& name = std::string());

    void deleteAsset(UUID id);

    template <typename Fn>
        requires std::invocable<Fn, AssetType, std::string_view>
    void forEachAssetType(Fn&& fn) const
    {
        for (int i = static_cast<int>(AssetType::Mesh); i < static_cast<int>(AssetType::Size); ++i)
        {
            fn(static_cast<AssetType>(i), AssetTypeNames[i]);
        }
    }

    template <typename Fn>
    void forEachByType(AssetType type, Fn&& fn) const
    {
        for (auto& [id, meta] : m_meta)
            if (meta.type == type) fn(id, meta);
    }

    std::unordered_map<UUID, AssetMetadata> getAllAssetsByType(AssetType type) const
    {
        std::unordered_map<UUID, AssetMetadata> result;
        for (const auto& [id, meta] : m_meta)
        {
            if (meta.type == type) result[id] = meta;
        }
        return result;
    }

    // serialize/deserialize registry file
  private:
    std::unordered_map<UUID, AssetMetadata> m_meta;

    std::optional<UUID> findAsset(const std::filesystem::path path) const;
};

} // namespace Engine