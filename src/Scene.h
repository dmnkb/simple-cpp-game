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

    // void OnRuntimeStart();
    // void OnRuntimeStop();

    // void OnSimulationStart();
    // void OnSimulationStop();

    // void OnUpdateRuntime(Timestep ts);
    // void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
    // void OnUpdateEditor(Timestep ts, EditorCamera& camera);
    // void OnViewportResize(uint32_t width, uint32_t height);

  private:
    std::vector<Ref<LightSceneNode>> m_LightSceneNodes;
    std::vector<Model> m_Models;
    Ref<Camera> m_DefaultCamera = nullptr;
    Ref<Camera> m_ActiveCamera = nullptr;
};