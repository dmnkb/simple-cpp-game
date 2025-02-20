#pragma once

#include "LightSceneNode.h"
#include "Model.h"
#include "pch.h"

// using SceneNodeVariant = std::variant<Ref<MeshSceneNode>, Ref<LightSceneNode>>;

struct SceneData
{
    std::vector<Ref<LightSceneNode>> lightSceneNodes;
    std::vector<Model> models;
    Ref<Camera> defaultCamera = nullptr;
    Ref<Camera> activeCamera = nullptr;
};

// using RenderPassFilter = std::function<bool(const Ref<MeshSceneNode>&)>;

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

class Scene
{
  public:
    static void init(const CameraProps& cameraProps);

    static void addModel(const Model& model);
    static void addLightSceneNode(const Ref<LightSceneNode>& node);

    // static std::optional<SceneNodeVariant> getByName(const std::string& name);
    // static RenderQueue getRenderQueue(const RenderPassFilter& filter);

    static RenderQueue getRenderQueue();
    static std::vector<Ref<LightSceneNode>> getLightSceneNodes();

    static void setActiveCamera(const Ref<Camera>& camera);
    static const Ref<Camera> getActiveCamera();
    static const Ref<Camera> getDefaultCamera();

    // void OnRuntimeStart();
    // void OnRuntimeStop();

    // void OnSimulationStart();
    // void OnSimulationStop();

    // void OnUpdateRuntime(Timestep ts);
    // void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
    // void OnUpdateEditor(Timestep ts, EditorCamera& camera);
    // void OnViewportResize(uint32_t width, uint32_t height);
};