#include "assets/AssetRegistry.h"
#include "assets/AssetHelpers.h"
#include "assets/AssetManager.h"
#include "pch.h"
#include "yaml-cpp/yaml.h"

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

    auto relativePath = path.is_absolute() ? std::filesystem::relative(path, AssetManager::getProjectRoot()) : path;

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

void AssetRegistry::serialize(const std::filesystem::path& filepath)
{
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;

    for (const auto& [id, meta] : m_meta)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "UUID" << YAML::Value << id.to_string();
        out << YAML::Key << "Type" << YAML::Value << static_cast<int>(meta.type);
        out << YAML::Key << "Name" << YAML::Value << meta.name;
        out << YAML::Key << "Path" << YAML::Value << meta.path.string();
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    auto absolutePath = AssetManager::toAbsolutePath(filepath);
    std::ofstream fout(absolutePath);
    fout << out.c_str();
}

void AssetRegistry::deserialize(const std::filesystem::path& filepath)
{
    YAML::Node data;
    auto absolutePath = AssetManager::toAbsolutePath(filepath);
    try
    {
        data = YAML::LoadFile(absolutePath.string());
    }
    catch (const std::exception& e)
    {
        std::cerr << "AssetRegistry: YAML::LoadFile failed for '" << filepath.string() << "': " << e.what() << "\n";
        return;
    }

    if (!data || !data.IsMap())
    {
        std::cerr << "AssetRegistry: Invalid registry file (root is not a map): " << filepath.string() << "\n";
        return;
    }

    auto assetsNode = data["Assets"];
    if (assetsNode && assetsNode.IsSequence())
    {
        for (const auto& assetNode : assetsNode)
        {
            if (!assetNode || !assetNode.IsMap()) continue;

            AssetMetadata meta;

            // UUID
            if (auto uuidNode = assetNode["UUID"])
            {
                if (auto uuidOpt = AssetHelpers::readUUID(uuidNode)) meta.uuid = *uuidOpt;
                else
                {
                    std::cerr << "AssetRegistry: Missing/invalid UUID in asset entry\n";
                    continue;
                }
            }

            // Type
            if (auto typeNode = assetNode["Type"]) meta.type = static_cast<AssetType>(typeNode.as<int>());

            // Name
            if (auto nameNode = assetNode["Name"]) meta.name = nameNode.as<std::string>();

            // Path
            if (auto pathNode = assetNode["Path"]) meta.path = pathNode.as<std::string>();

            m_meta[meta.uuid] = meta;
        }
    }
}

} // namespace Engine