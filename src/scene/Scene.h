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

    RenderQueue getRenderQueue(const std::string& passName);
    std::vector<Ref<SpotLight>> getSpotLights() const;
    std::vector<Ref<PointLight>> getPointLights() const;
    Ref<DirectionalLight> getDirectionalLight() const;

    // TODO: Remove once ECS is in place:
    Ref<SpotLight> getSpotLight(uuids::uuid identifier) const;

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

    RenderQueue m_cachedRenderQueue;
    bool m_renderablesDirty = true;

    Ref<Camera> m_DefaultCamera = nullptr;
    Ref<Camera> m_ActiveCamera = nullptr;
};

} // namespace Engine