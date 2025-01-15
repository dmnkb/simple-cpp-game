#pragma once

#include "Light.h"
#include "LightSceneNode.h"
#include "MeshSceneNode.h"
#include "Renderer.h"
#include "pch.h"

const unsigned int MAX_LIGHTS = 3;

using SceneNodeVariant = std::variant<Ref<MeshSceneNode>, Ref<LightSceneNode>>;

struct SceneData
{
    std::vector<Ref<LightSceneNode>> lightSceneNodes;
    std::vector<Ref<MeshSceneNode>> meshSceneNodes;
    RenderQueue renderQueue = {};
    Ref<Camera> defaultCamera = nullptr;
    Ref<Camera> activeCamera = nullptr;
};

using RenderPassFilter = std::function<bool(const Ref<MeshSceneNode>&)>;

class Scene
{
  public:
    static void init(const CameraProps& cameraProps);
    static void addMeshSceneNode(const Ref<MeshSceneNode>& node);
    static void addLightSceneNode(const Ref<LightSceneNode>& node);

    static std::optional<SceneNodeVariant> getByName(const std::string& name);
    static RenderQueue& getRenderQueue(const RenderPassFilter& filter);
    static void clearRenderQueue();
    static std::vector<Ref<LightSceneNode>> getLightSceneNodes();

    static void setActiveCamera(const Ref<Camera>& camera);
    static const Ref<Camera> getActiveCamera();
    static const Ref<Camera> getDefaultCamera();
};