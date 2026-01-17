#pragma once

#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "assets/AssetHelpers.h"
#include "assets/AssetManager.h"
#include "core/UUID.h"
#include "scene/Entity.h"
#include "scene/Scene.h"

namespace Engine
{

struct SceneSerializer
{

    // MARK: Serialize Entity
    static void serializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "EntityID" << YAML::Value << static_cast<uint32_t>(entity.getHandle());

        // TagComponent
        if (entity.hasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent" << YAML::Value << YAML::BeginMap;
            auto& tagComp = entity.getComponent<TagComponent>();
            out << YAML::Key << "Tag" << YAML::Value << tagComp.tag;
            out << YAML::EndMap;
        }

        // TransformComponent
        if (entity.hasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent" << YAML::Value << YAML::BeginMap;
            auto& tc = entity.getComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.scale;
            out << YAML::EndMap;
        }

        // MeshComponent
        if (entity.hasComponent<MeshComponent>())
        {
            auto& mc = entity.getComponent<MeshComponent>();

            out << YAML::Key << "MeshComponent" << YAML::Value << YAML::BeginMap;

            // Mesh reference (optional)
            out << YAML::Key << "MeshUUID" << YAML::Value;
            if (mc.mesh) AssetHelpers::writeUUID(out, mc.mesh->metadata.uuid);
            else out << YAML::Null;

            // Override materials (optional per-slot)
            out << YAML::Key << "OverrideMaterials" << YAML::Value << YAML::BeginSeq;
            for (const auto& mat : mc.overrideMaterials)
            {
                if (mat) out << mat->metadata.uuid.to_string();
                else out << YAML::Null;
            }
            out << YAML::EndSeq;

            out << YAML::EndMap;
        }

        // PointLightComponent
        if (entity.hasComponent<PointLightComponent>())
        {
            auto& plc = entity.getComponent<PointLightComponent>();
            out << YAML::Key << "PointLightComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Color" << YAML::Value << plc.color;
            out << YAML::Key << "Intensity" << YAML::Value << plc.intensity;
            out << YAML::Key << "Radius" << YAML::Value << plc.radius;
            out << YAML::Key << "Constant" << YAML::Value << plc.constant;
            out << YAML::Key << "Linear" << YAML::Value << plc.linear;
            out << YAML::Key << "Quadratic" << YAML::Value << plc.quadratic;
            out << YAML::EndMap;
        }

        // SpotLightComponent
        if (entity.hasComponent<SpotLightComponent>())
        {
            auto& slc = entity.getComponent<SpotLightComponent>();
            out << YAML::Key << "SpotLightComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Color" << YAML::Value << slc.color;
            out << YAML::Key << "Intensity" << YAML::Value << slc.intensity;
            out << YAML::Key << "InnerConeAngle" << YAML::Value << slc.innerConeAngle;
            out << YAML::Key << "OuterConeAngle" << YAML::Value << slc.outerConeAngle;
            out << YAML::Key << "Constant" << YAML::Value << slc.constant;
            out << YAML::Key << "Linear" << YAML::Value << slc.linear;
            out << YAML::Key << "Quadratic" << YAML::Value << slc.quadratic;
            out << YAML::EndMap;
        }

        // DirectionalLightComponent
        if (entity.hasComponent<DirectionalLightComponent>())
        {
            auto& dlc = entity.getComponent<DirectionalLightComponent>();
            out << YAML::Key << "DirectionalLightComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Color" << YAML::Value << dlc.color;
            out << YAML::Key << "Intensity" << YAML::Value << dlc.intensity;
            out << YAML::Key << "IsMainLight" << YAML::Value << dlc.isMainLight;
            out << YAML::EndMap;
        }

        out << YAML::EndMap;
    }

    // MARK: Serialize Scene
    static bool serialize(const Ref<Scene>& scene, const std::filesystem::path& filepath)
    {
        if (!scene)
        {
            std::cerr << "SceneSerializer: serialize() called with null scene\n";
            return false;
        }

        auto absolutePath = AssetManager::toAbsolutePath(filepath);
        YAML::Emitter out;

        out << YAML::BeginMap;

        // Metadata
        out << YAML::Key << "Metadata" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Type" << YAML::Value << static_cast<int>(scene->metadata.type);
        out << YAML::Key << "Name" << YAML::Value << scene->metadata.name;
        out << YAML::Key << "Path" << YAML::Value << scene->metadata.path.string();
        out << YAML::Key << "UUID" << YAML::Value << scene->metadata.uuid.to_string();
        out << YAML::EndMap;

        // Entities
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        for (auto entityID : scene->m_registry.storage<entt::entity>())
        {
            Entity entity{entityID, scene.get()};
            if (!entity) continue;
            serializeEntity(out, entity);
        }

        out << YAML::EndSeq;

        // Lighting
        out << YAML::Key << "AmbientLightColor" << YAML::Value << scene->getAmbientLightColor();
        out << YAML::Key << "SunLightColor" << YAML::Value << scene->getSunLightColor();

        out << YAML::EndMap;

        std::ofstream fout(absolutePath);
        if (!fout.is_open())
        {
            std::cerr << "SceneSerializer: Failed to open for writing: " << absolutePath.string() << "\n";
            return false;
        }

        fout << out.c_str();
        return fout.good();
    }

    // MARK: Deserialize Scene
    static bool deserialize(const Ref<Scene>& scene, const std::filesystem::path& filepath)
    {
        if (!scene)
        {
            std::cerr << "SceneSerializer: deserialize() called with null scene\n";
            return false;
        }

        auto absolutePath = AssetManager::toAbsolutePath(filepath);
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(absolutePath.string());
        }
        catch (const std::exception& e)
        {
            std::cerr << "SceneSerializer: YAML::LoadFile failed for '" << absolutePath.string() << "': " << e.what()
                      << "\n";
            return false;
        }

        if (!data || !data.IsMap())
        {
            std::cerr << "SceneSerializer: Invalid scene file (root is not a map): " << filepath.string() << "\n";
            return false;
        }

        // Metadata (note: key is "Metadata" with capital M)
        if (auto metaNode = data["Metadata"])
        {
            if (metaNode["Type"]) scene->metadata.type = static_cast<AssetType>(metaNode["Type"].as<int>());
            if (metaNode["Name"]) scene->metadata.name = metaNode["Name"].as<std::string>();
            if (metaNode["Path"]) scene->metadata.path = metaNode["Path"].as<std::string>();

            if (auto uuidOpt = AssetHelpers::readUUID(metaNode["UUID"])) scene->metadata.uuid = *uuidOpt;
            else
            {
                std::cerr << "SceneSerializer: Missing/invalid UUID in scene metadata\n";
                return false;
            }
        }

        // Lighting
        if (auto amb = data["AmbientLightColor"]; amb && amb.IsSequence())
            scene->setAmbientLightColor(amb.as<glm::vec4>());

        if (auto sun = data["SunLightColor"]; sun && sun.IsSequence()) scene->setSunLightColor(sun.as<glm::vec4>());

        // Entities
        auto entitiesNode = data["Entities"];
        if (entitiesNode && entitiesNode.IsSequence())
        {
            for (const auto& entityNode : entitiesNode)
            {
                if (!entityNode || !entityNode.IsMap()) continue;

                // You currently store EntityID, but your Scene::createEntity() API does not accept it.
                // So we create a new entity and deserialize components onto it.
                Entity entity = scene->createEntity();

                // TagComponent
                if (auto tagNode = entityNode["TagComponent"])
                {
                    auto& tc = entity.getComponent<TagComponent>();
                    if (tagNode["Tag"]) tc.tag = tagNode["Tag"].as<std::string>();
                }

                // TransformComponent
                if (auto trNode = entityNode["TransformComponent"])
                {
                    if (!entity.hasComponent<TransformComponent>()) entity.addComponent<TransformComponent>();

                    auto& tr = entity.getComponent<TransformComponent>();
                    if (trNode["Translation"]) tr.translation = trNode["Translation"].as<glm::vec3>();
                    if (trNode["Rotation"]) tr.rotation = trNode["Rotation"].as<glm::vec3>();
                    if (trNode["Scale"]) tr.scale = trNode["Scale"].as<glm::vec3>();
                }

                // MeshComponent
                if (auto meshNode = entityNode["MeshComponent"])
                {
                    if (!entity.hasComponent<MeshComponent>()) entity.addComponent<MeshComponent>();

                    auto& mc = entity.getComponent<MeshComponent>();

                    // Mesh UUID -> load mesh asset
                    if (auto meshUUID = AssetHelpers::readUUID(meshNode["MeshUUID"]))
                    {
                        mc.mesh = AssetManager::getOrImport<Mesh>(*meshUUID);
                    }
                    else
                    {
                        mc.mesh.reset();
                    }

                    // Override materials
                    mc.overrideMaterials.clear();
                    if (auto overrides = meshNode["OverrideMaterials"]; overrides && overrides.IsSequence())
                    {
                        mc.overrideMaterials.reserve(overrides.size());
                        for (const auto& n : overrides)
                        {
                            if (auto matUUID = AssetHelpers::readUUID(n))
                                mc.overrideMaterials.emplace_back(AssetManager::getOrImport<Material>(*matUUID));
                            else mc.overrideMaterials.emplace_back(nullptr);
                        }
                    }

                    // Optional: Ensure overrides vector matches slot count if mesh is present
                    if (mc.mesh)
                    {
                        const size_t slotCount = mc.mesh->defaultMaterialSlots.size();
                        if (mc.overrideMaterials.size() < slotCount) mc.overrideMaterials.resize(slotCount, nullptr);
                    }
                }
            }
        }

        return true;
    }
};

} // namespace Engine
