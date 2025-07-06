#pragma once

#include "LightSceneNode.h"
#include "Model.h"
#include "pch.h"

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

class Scene
{
  public:
    Scene() = default;
    void init(const CameraProps& cameraProps);

    void addModel(const Model& model);
    void addLightSceneNode(const Ref<LightSceneNode>& node);

    RenderQueue getRenderQueue();
    std::vector<Ref<LightSceneNode>> getLightSceneNodes();

    void setActiveCamera(const Ref<Camera>& camera);
    Ref<Camera> getActiveCamera() const;
    Ref<Camera> getDefaultCamera() const;

  private:
    std::vector<Ref<LightSceneNode>> m_LightSceneNodes;
    std::vector<Model> m_Models;
    // TODO: idea:
    // std::vector<Renderable> m_cachedRenderables;
    // bool m_renderablesDirty = true;
    Ref<Camera> m_DefaultCamera = nullptr;
    Ref<Camera> m_ActiveCamera = nullptr;
};