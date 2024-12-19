#include "Scene.h"
#include "LightSceneNode.h"
#include "MeshSceneNode.h"
#include "Renderer.h"

static SceneData s_sceneData;

void Scene::update()
{
    submitLights();
    submitMeshSceneNodes();
}

void Scene::addMeshSceneNode(const std::shared_ptr<MeshSceneNode>& node)
{
    s_sceneData.meshSceneNodes.push_back(node);
}

void Scene::addLightSceneNode(const std::shared_ptr<LightSceneNode>& node)
{
    s_sceneData.lightSceneNodes.push_back(node);
}

std::optional<SceneNodeVariant> Scene::getByName(const std::string& name)
{
    auto meshSceneNodeIt =
        std::find_if(s_sceneData.meshSceneNodes.begin(), s_sceneData.meshSceneNodes.end(),
                     [&name](const std::shared_ptr<MeshSceneNode>& node) { return node->getName() == name; });

    if (meshSceneNodeIt != s_sceneData.meshSceneNodes.end())
    {
        return (*meshSceneNodeIt);
    }

    auto lightSceneNodeIt =
        std::find_if(s_sceneData.lightSceneNodes.begin(), s_sceneData.lightSceneNodes.end(),
                     [&name](const std::shared_ptr<LightSceneNode>& node) { return node->getName() == name; });

    if (lightSceneNodeIt != s_sceneData.lightSceneNodes.end())
    {
        return (*lightSceneNodeIt);
    }

    return std::nullopt;
}

void Scene::submitLights()
{
    for (const auto& node : s_sceneData.lightSceneNodes)
    {
        Renderer::submitLight(node->prepareLight());
    }
}

void Scene::submitMeshSceneNodes()
{
    for (const auto& node : s_sceneData.meshSceneNodes)
    {
        Renderer::submitRenderable(node->prepareRenderable());
    }
}