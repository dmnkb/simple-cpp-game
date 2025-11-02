#pragma once

#include "core/Core.h"
#include "renderer/Camera.h"
#include "renderer/DirectionalLight.h"
#include "renderer/Model.h"
#include "renderer/PointLight.h"
#include "renderer/SpotLight.h"
#include "uuid/uuid.h"

namespace Engine
{

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

class Scene
{
  public:
    Scene() = default;
    void init(const Camera::CameraProps& cameraProps);

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
        for (const auto& light : m_SpotLights)
        {
            if (light->getIdentifier() == identifier)
                return light;
        }
        return nullptr;
    }

    Ref<PointLight> getPointLightByID(UUID identifier) const
    {
        for (const auto& light : m_PointLights)
        {
            if (light->getIdentifier() == identifier)
                return light;
        }
        return nullptr;
    }

    Ref<DirectionalLight> getDirectionalLightByID(UUID identifier) const
    {
        if (m_DirectionalLight && m_DirectionalLight->getIdentifier() == identifier)
            return m_DirectionalLight;
        return nullptr;
    }

    void setActiveCamera(const Ref<Camera>& camera);
    Ref<Camera> getActiveCamera() const;
    Ref<Camera> getDefaultCamera() const;

  private:
    // TODO:
    // entt::registry m_registry;

    std::vector<Ref<SpotLight>> m_SpotLights;
    std::vector<Ref<PointLight>> m_PointLights;
    Ref<DirectionalLight> m_DirectionalLight;
    std::vector<Model> m_Models;

    glm::vec4 ambientLightColor = {0.1f, 0.1f, 0.1f, 1.0f};

    RenderQueue m_cachedRenderQueue;
    bool m_renderablesDirty = true;

    Ref<Camera> m_DefaultCamera = nullptr;
    Ref<Camera> m_ActiveCamera = nullptr;
};

} // namespace Engine