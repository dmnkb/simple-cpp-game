#pragma once

#include <concepts>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "renderer/Material.h"
#include "renderer/Mesh.h"
#include <glm/gtx/quaternion.hpp>

namespace Engine
{

struct TagComponent
{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : tag(tag) {}
};

struct TransformComponent
{
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f}; // In degrees
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation) : translation(translation) {}

    glm::mat4 getTransform() const
    {
        glm::mat4 rotationMatrix = glm::toMat4(glm::quat(glm::radians(rotation)));
        return glm::translate(glm::mat4(1.0f), translation) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
    }
};

struct MeshComponent
{
    Ref<Mesh> mesh;
    std::vector<Ref<Material>> materials;

    MeshComponent() = default;
    MeshComponent(const MeshComponent&) = default;
    MeshComponent(const Ref<Mesh>& mesh, const std::vector<Ref<Material>>& materials) : mesh(mesh), materials(materials)
    {
    }
};

struct PointLightComponent
{
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float radius = 10.0f;
    // Attenuation factors
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent&) = default;
};

struct SpotLightComponent
{
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    float innerConeAngle = glm::radians(15.0f);
    float outerConeAngle = glm::radians(25.0f);
    // Attenuation factors
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    SpotLightComponent() = default;
    SpotLightComponent(const SpotLightComponent&) = default;
};

struct DirectionalLightComponent
{
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;
    bool isMainLight = false;

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent&) = default;
};

// Concept to constrain light component types
template <typename T>
concept LightComponent = std::same_as<T, PointLightComponent> || std::same_as<T, SpotLightComponent> ||
                         std::same_as<T, DirectionalLightComponent>;

} // namespace Engine