#pragma once

#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "assets/AssetManager.h"
#include "renderer/Material.h"

namespace Engine
{

struct MaterialSerializer
{
    static bool serialize(const Ref<Material>& material, const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "shader" << YAML::Value << material->getShader()->metadata.uuid.to_string();

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

        if (auto shaderUuidOpt = UUID::from_string(data["shader"].as<std::string>()))
        {
            UUID shaderUuid = *shaderUuidOpt;
            Ref<Shader> shader = AssetManager::getOrImport<Shader>(shaderUuid);
            if (shader)
            {
                material->shader = shader;
            }
            else
            {
                std::cerr << "MaterialSerializer: Shader UUID in material didn't resolve to a valid shader. Using "
                             "default shader\n";
                material->shader = Shader::getStandardShader();
            }
        }
        else
        {
            std::cerr << "MaterialSerializer: shader UUID missing. Using default shader\n";
            material->shader = Shader::getStandardShader();
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

        return true;
    }
};

}; // namespace Engine
