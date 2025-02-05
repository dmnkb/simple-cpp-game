#include "Scene.h"

static SceneData s_sceneData;

void Scene::init(const CameraProps& cameraProps)
{
    s_sceneData.defaultCamera = CreateRef<Camera>(cameraProps);
    setActiveCamera(s_sceneData.defaultCamera);
}

void Scene::addMeshSceneNode(const Ref<MeshSceneNode>& node)
{
    s_sceneData.meshSceneNodes.push_back(node);
}

void Scene::addLightSceneNode(const Ref<LightSceneNode>& node)
{
    s_sceneData.lightSceneNodes.push_back(node);
}

std::optional<SceneNodeVariant> Scene::getByName(const std::string& name)
{
    auto meshSceneNodeIt = std::find_if(s_sceneData.meshSceneNodes.begin(), s_sceneData.meshSceneNodes.end(),
                                        [&name](const Ref<MeshSceneNode>& node) { return node->getName() == name; });

    if (meshSceneNodeIt != s_sceneData.meshSceneNodes.end())
    {
        return (*meshSceneNodeIt);
    }

    auto lightSceneNodeIt = std::find_if(s_sceneData.lightSceneNodes.begin(), s_sceneData.lightSceneNodes.end(),
                                         [&name](const Ref<LightSceneNode>& node) { return node->getName() == name; });

    if (lightSceneNodeIt != s_sceneData.lightSceneNodes.end())
    {
        return (*lightSceneNodeIt);
    }

    return std::nullopt;
}

/**
 * TODO:
 * For more complex scenes, implement a culling stage that processes the scene graph and produces
 * Renderables only for visible objects. This can be integrated with spatial partitioning techniques like BVH or
 * octrees.
 *
 * Example:
 * std::vector<Renderable*> candidates = spatialPartition.Query(cameraFrustum);
 */
RenderQueue Scene::getRenderQueue(const RenderPassFilter& filter)
{
    RenderQueue renderQueue = {};

    for (const auto& node : s_sceneData.meshSceneNodes)
    {
        if (filter(node))
        {
            auto renderable = node->getRenderable();
            renderQueue[renderable->material][renderable->mesh].push_back(renderable->transform);
        }
    }
    return renderQueue;
}

std::vector<Ref<LightSceneNode>> Scene::getLightSceneNodes()
{
    return s_sceneData.lightSceneNodes;
}

void Scene::setActiveCamera(const Ref<Camera>& camera)
{
    s_sceneData.activeCamera = camera;
}

const Ref<Camera> Scene::getActiveCamera()
{
    return s_sceneData.activeCamera;
}

const Ref<Camera> Scene::getDefaultCamera()
{
    return s_sceneData.defaultCamera;
}