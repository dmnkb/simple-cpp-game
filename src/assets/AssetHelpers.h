#pragma once

#include "yaml-cpp/yaml.h"
#include <optional>
#include <string>

#include "assets/Asset.h"
#include "core/Core.h"
#include "core/UUID.h"

namespace Engine
{

struct AssetHelpers
{
    static std::optional<UUID> readUUID(const YAML::Node& node)
    {
        if (!node || node.IsNull()) return std::nullopt;
        if (!node.IsScalar()) return std::nullopt;

        const auto s = node.as<std::string>();
        if (auto u = UUID::from_string(s)) return *u;
        return std::nullopt;
    }

    static void writeUUID(YAML::Emitter& out, const UUID& uuid) { out << uuid.to_string(); }

    static void writeUUIDOrNull(YAML::Emitter& out, const Ref<Asset>& asset)
    {
        if (asset) out << asset->metadata.uuid.to_string();
        else out << YAML::Null;
    }
};

} // namespace Engine