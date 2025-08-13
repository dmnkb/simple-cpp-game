#pragma once

#include "core/Core.h"
#include "renderer/LightSceneNode.h"
#include "renderer/Model.h"

namespace Engine
{

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

class Scene
{
  public:
    Scene() = default;
    void init(const CameraProps& cameraProps);

    void addModel(const Model& model);
    void addLightSceneNode(const Ref<LightSceneNode>& node);

    RenderQueue getRenderQueue(const std::string& passName);
    std::vector<Ref<LightSceneNode>> getLightSceneNodes() const;

    void setActiveCamera(const Ref<Camera>& camera);
    Ref<Camera> getActiveCamera() const;
    Ref<Camera> getDefaultCamera() const;

  private:
    std::vector<Ref<LightSceneNode>> m_LightSceneNodes;
    std::vector<Model> m_Models;

    RenderQueue m_cachedRenderQueue;
    bool m_renderablesDirty = true;

    Ref<Camera> m_DefaultCamera = nullptr;
    Ref<Camera> m_ActiveCamera = nullptr;
};

} // namespace Engine