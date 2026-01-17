#pragma once

#include "yaml-cpp/yaml.h"
#include <optional>
#include <string>

#include "assets/Asset.h"
#include "core/Core.h"
#include "core/UUID.h"
#include "glm/glm.hpp"

namespace YAML
{
template <>
struct convert<glm::vec3>
{
    static Node encode(const glm::vec3& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3) return false;
        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template <>
struct convert<glm::vec4>
{
    static Node encode(const glm::vec4& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        return node;
    }

    static bool decode(const Node& node, glm::vec4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4) return false;
        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

inline Emitter& operator<<(Emitter& out, const glm::vec3& v)
{
    out << Flow;
    out << BeginSeq << v.x << v.y << v.z << EndSeq;
    return out;
}

inline Emitter& operator<<(Emitter& out, const glm::vec4& v)
{
    out << Flow;
    out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
    return out;
}
} // namespace YAML

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