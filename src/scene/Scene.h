#pragma once

#include "core/Core.h"
#include "renderer/Camera.h"
#include "renderer/DirectionalLight.h"
#include "renderer/Model.h"
#include "renderer/PointLight.h"
#include "renderer/SpotLight.h"
#include "util/uuid.h"

namespace Engine
{

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

class Scene
{
  public:
    Scene();

    void addModel(const Model& model);
    void addSpotLight(const Ref<SpotLight>& light);
    void addPointLight(const Ref<PointLight>& light);
    void setDirectionalLight(const Ref<DirectionalLight>& light);
    void setAmbientLightColor(const glm::vec4& color);
    glm::vec4 getAmbientLightColor() const;

    RenderQueue getRenderQueue(const std::string& passName);
    std::vector<Ref<SpotLight>> getSpotLights() const;
    std::vector<Ref<PointLight>> getPointLights() const;
    Ref<DirectionalLight> getDirectionalLight() const;

    // TODO: Remove once ECS is in place:
    Ref<SpotLight> getSpotLightByID(UUID identifier) const
    {
        for (const auto& light : m_spotLights)
        {
            if (light->getIdentifier() == identifier)
                return light;
        }
        return nullptr;
    }

    Ref<PointLight> getPointLightByID(UUID identifier) const
    {
        for (const auto& light : m_pointLights)
        {
            if (light->getIdentifier() == identifier)
                return light;
        }
        return nullptr;
    }

    Ref<DirectionalLight> getDirectionalLightByID(UUID identifier) const
    {
        if (m_directionalLight && m_directionalLight->getIdentifier() == identifier)
            return m_directionalLight;
        return nullptr;
    }

    void setActiveCamera(const Camera& camera);
    Camera& getActiveCamera();

  private:
    // TODO:
    // entt::registry m_registry;

    std::vector<Ref<SpotLight>> m_spotLights;
    std::vector<Ref<PointLight>> m_pointLights;
    Ref<DirectionalLight> m_directionalLight;
    std::vector<Model> m_models;

    glm::vec4 m_ambientLightColor = {0.302f, 0.329f, 0.357f, 1.0f};
    glm::vec4 m_sunLightColor = {0.925f, 0.886f, 0.804f, 0.702f};

    RenderQueue m_cachedRenderQueue;
    bool m_renderablesDirty = true;

    Camera m_activeCamera;
};

} // namespace Engine