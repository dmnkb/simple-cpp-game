#pragma once

#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "assets/AssetManager.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

namespace Engine
{

} // namespace Engine

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

struct SceneSerializer
{
    // MARK: Serialize
    static void serializeEntity(YAML::Emitter& out, Entity& entity)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "EntityID" << YAML::Value << static_cast<uint32_t>(entity.getHandle());

        // MARK: TagComponent
        if (entity.hasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::Value;
            out << YAML::BeginMap;
            auto& tagComp = entity.getComponent<TagComponent>();
            out << YAML::Key << "Tag" << YAML::Value << tagComp.tag;
            out << YAML::EndMap;
        }

        // MARK: TransformComponent
        if (entity.hasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::Value;
            out << YAML::BeginMap;
            auto& transformComp = entity.getComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << transformComp.translation;
            out << YAML::Key << "Rotation" << YAML::Value << transformComp.rotation;
            out << YAML::Key << "Scale" << YAML::Value << transformComp.scale;
            out << YAML::EndMap;
        }

        // MARK: MeshComponent
        if (entity.hasComponent<MeshComponent>())
        {
            out << YAML::Key << "MeshComponent";
            out << YAML::Value;
            out << YAML::BeginMap;
            auto& meshComp = entity.getComponent<MeshComponent>();
            out << YAML::Key << "MeshUUID" << YAML::Value << meshComp.mesh->metadata.uuid.to_string();
            // Serialize override materials if any
            out << YAML::Key << "OverrideMaterials";
            out << YAML::Value << YAML::BeginSeq;
            for (const auto& material : meshComp.overrideMaterials)
            {
                if (material) out << material->metadata.uuid.to_string();
                else out << "null";
            }
            out << YAML::EndSeq;
            out << YAML::EndMap;
        }

        out << YAML::EndMap;
    }

    static bool serialize(const Ref<Scene>& scene, const std::filesystem::path& filepath)
    {
        YAML::Emitter out;

        out << YAML::BeginMap;
        out << YAML::Key << "Metadata" << YAML::Value;
        out << YAML::BeginMap;
        out << YAML::Key << "Type" << YAML::Value << static_cast<int>(scene->metadata.type);
        out << YAML::Key << "Name" << YAML::Value << scene->metadata.name;
        out << YAML::Key << "Path" << YAML::Value << scene->metadata.path.string();
        out << YAML::Key << "UUID" << YAML::Value << scene->metadata.uuid.to_string();
        out << YAML::EndMap;

        out << YAML::Key << "Entities";
        out << YAML::Value << YAML::BeginSeq;

        for (auto entityID : scene->m_registry.storage<entt::entity>())
        {
            Entity entity{entityID, scene.get()};
            if (!entity) continue;

            serializeEntity(out, entity);
        }

        out << YAML::EndSeq;

        out << YAML::Key << "AmbientLightColor" << YAML::Value << scene->getAmbientLightColor();
        out << YAML::Key << "SunLightColor" << YAML::Value << scene->getSunLightColor();

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        return fout.good();
    }

    static bool deserialize(const Ref<Scene>& scene, const std::filesystem::path& filepath)
    {
        YAML::Node data = YAML::LoadFile(filepath.string());
        if (!data)
        {
            std::cerr << "SceneSerializer: Failed to load scene from '" << filepath.string() << "'\n";
            return false;
        }

        auto metaNode = data["metadata"];
        if (metaNode)
        {
            scene->metadata.type = static_cast<AssetType>(metaNode["Type"].as<int>());
            scene->metadata.name = metaNode["Name"].as<std::string>();
            scene->metadata.path = metaNode["Path"].as<std::string>();
            if (auto uuidOpt = UUID::from_string(metaNode["UUID"].as<std::string>()))
            {
                scene->metadata.uuid = *uuidOpt;
            }
            else
            {
                std::cerr << "SceneSerializer: Invalid UUID in scene metadata\n";
                return false;
            }
        }

        auto entitiesNode = data["entities"];
        if (entitiesNode && entitiesNode.IsSequence())
        {
            for (const auto& entityNode : entitiesNode)
            {
                uint32_t entityID = entityNode["EntityID"].as<uint32_t>();
                Entity entity = scene->createEntity();

                // TODO: Finish deserializing components here
            }
        }

        return true;
    }
};

}; // namespace Engine
