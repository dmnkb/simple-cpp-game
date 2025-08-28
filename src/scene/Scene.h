#pragma once

#include "core/Core.h"
#include "renderer/Camera.h"
#include "renderer/Model.h"
#include "renderer/SpotLight.h"

namespace Engine
{

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

class Scene
{
  public:
    Scene() = default;
    void init(const CameraProps& cameraProps);

    void addModel(const Model& model);
    void addSpotLight(const Ref<SpotLight>& light);

    RenderQueue getRenderQueue(const std::string& passName);
    std::vector<Ref<SpotLight>> getSpotLights() const;

    void setActiveCamera(const Ref<Camera>& camera);
    Ref<Camera> getActiveCamera() const;
    Ref<Camera> getDefaultCamera() const;

  private:
    std::vector<Ref<SpotLight>> m_SpotLights;
    std::vector<Model> m_Models;

    RenderQueue m_cachedRenderQueue;
    bool m_renderablesDirty = true;

    Ref<Camera> m_DefaultCamera = nullptr;
    Ref<Camera> m_ActiveCamera = nullptr;
};

} // namespace Engine