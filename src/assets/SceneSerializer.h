#pragma once

#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "assets/AssetManager.h"
#include "scene/Scene.h"

namespace Engine
{

struct SceneSerializer
{
    static bool serialize(const Ref<Scene>& scene, const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "metadata" << YAML::Value;
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << static_cast<int>(scene->metadata.type);
        out << YAML::Key << "name" << YAML::Value << scene->metadata.name;
        out << YAML::Key << "path" << YAML::Value << scene->metadata.path.string();
        out << YAML::Key << "uuid" << YAML::Value << scene->metadata.uuid.to_string();
        out << YAML::EndMap;

        // TODO: m_ambientLightColor, m_sunLightColor, registry

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
            scene->metadata.type = static_cast<AssetType>(metaNode["type"].as<int>());
            scene->metadata.name = metaNode["name"].as<std::string>();
            scene->metadata.path = metaNode["path"].as<std::string>();
            if (auto uuidOpt = UUID::from_string(metaNode["uuid"].as<std::string>()))
            {
                scene->metadata.uuid = *uuidOpt;
            }
            else
            {
                std::cerr << "SceneSerializer: Invalid UUID in scene metadata\n";
                return false;
            }
        }

        return true;
    }
};

}; // namespace Engine
