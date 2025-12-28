#pragma once

#include "vendor/yaml-cpp/include/yaml-cpp/yaml.h"
#include <filesystem>
#include <fstream>

#include "renderer/Material.h"

namespace Engine
{

struct MaterialSerializer
{
    static bool serialize(const Ref<Material>& material, const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "metadata" << YAML::Value;
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << static_cast<int>(material->metadata.type);
        out << YAML::Key << "name" << YAML::Value << material->metadata.name;
        out << YAML::Key << "path" << YAML::Value << material->metadata.path.string();
        out << YAML::Key << "uuid" << YAML::Value << material->metadata.uuid.to_string();
        out << YAML::EndMap;

        out << YAML::Key << "isDoubleSided" << YAML::Value << material->isDoubleSided;
        out << YAML::Key << "alphaMode" << YAML::Value << static_cast<int>(material->alphaMode);
        out << YAML::Key << "alphaCutoff" << YAML::Value << material->alphaCutoff;

        // Serialize other properties as needed...

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        return fout.good();
    }

    static bool deserialize(const Ref<Material>& material, const std::filesystem::path& filepath)
    {
        YAML::Node data = YAML::LoadFile(filepath.string());
        if (!data)
        {
            std::cerr << "MaterialSerializer: Failed to load material from '" << filepath.string() << "'\n";
            return false;
        }

        auto metaNode = data["metadata"];
        if (metaNode)
        {
            material->metadata.type = static_cast<AssetType>(metaNode["type"].as<int>());
            material->metadata.name = metaNode["name"].as<std::string>();
            material->metadata.path = metaNode["path"].as<std::string>();
            if (auto uuidOpt = UUID::from_string(metaNode["uuid"].as<std::string>()))
            {
                material->metadata.uuid = *uuidOpt;
            }
            else
            {
                std::cerr << "MaterialSerializer: Invalid UUID in material metadata\n";
                return false;
            }
        }

        material->isDoubleSided = data["isDoubleSided"].as<bool>(false);
        material->alphaMode = static_cast<AlphaMode>(data["alphaMode"].as<int>(0));
        material->alphaCutoff = data["alphaCutoff"].as<float>(0.5f);

        // Deserialize other properties as needed...

        return true;
    }
};

}; // namespace Engine
