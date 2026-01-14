#include "assets/AssetRegistry.h"
#include "pch.h"

namespace Engine
{

const AssetMetadata* AssetRegistry::find(UUID id)
{
    auto it = m_meta.find(id);
    if (it != m_meta.end()) return &it->second;
    return nullptr;
}

UUID AssetRegistry::findOrRegisterAsset(AssetType type, const std::filesystem::path path, const std::string& name)
{
    std::println("AssetRegistry: find / reg asset '{}'", name);

    // Normalize to relative path for storage
    auto relativePath = path.is_absolute() ? std::filesystem::relative(path, std::filesystem::current_path()) : path;

    // If already registered, return existing ID
    if (auto existingId = findAsset(relativePath); existingId.has_value()) return *existingId;

    // Register new asset
    UUID newId = UUID::random();
    AssetMetadata meta;
    meta.type = type;
    meta.uuid = newId;
    if (!name.empty()) meta.name = name;
    if (!path.empty()) meta.path = relativePath;

    m_meta[newId] = meta;
    // TODO: serialize registry to disk? -> On "save project"?

    std::println("Registered new asset: {} (type={}, path='{}')", meta.name, static_cast<int>(type),
                 meta.path.string());

    return newId;
}

void AssetRegistry::unregisterAssetByID(UUID id)
{
    m_meta.erase(id);
}

std::optional<UUID> AssetRegistry::findAsset(const std::filesystem::path path)
{
    for (const auto& [id, meta] : m_meta)
    {
        if (meta.path == path) return id;
    }
    return std::nullopt;
}

} // namespace Engine