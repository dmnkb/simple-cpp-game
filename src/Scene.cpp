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