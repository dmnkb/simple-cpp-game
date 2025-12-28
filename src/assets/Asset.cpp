#include "assets/Asset.h"

#include <filesystem>
#include <iostream>

namespace Engine
{

// AssetRegistry methods
const AssetMetadata* AssetRegistry::find(UUID id) const
{
    auto it = m_meta.find(id);
    if (it != m_meta.end()) return &it->second;
    return nullptr;
}

UUID AssetRegistry::findOrRegisterAsset(AssetType type, const std::filesystem::path path, const std::string& name)
{
    // If already registered, return existing ID
    if (auto existingId = findAsset(path); existingId.has_value()) return *existingId;

    // Register new asset
    UUID newId = UUID::random();
    AssetMetadata meta;
    meta.type = type;
    meta.uuid = newId;
    if (!name.empty()) meta.name = name;
    if (!path.empty()) meta.path = path;

    m_meta[newId] = meta;
    // TODO: serialize registry to disk?

    std::println("Registered new asset: {} (type={}, path='{}')", meta.name, static_cast<int>(type),
                 meta.path.string());

    return newId;
}

void AssetRegistry::deleteAsset(UUID id)
{
    m_meta.erase(id);
    // TODO: serialize registry to disk?
}

std::optional<UUID> AssetRegistry::findAsset(const std::filesystem::path path) const
{
    for (const auto& [id, meta] : m_meta)
    {
        if (meta.path == path) return id;
    }
    return std::nullopt;
}

} // namespace Engine
