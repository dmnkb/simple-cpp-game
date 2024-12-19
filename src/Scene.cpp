#include "Scene.h"
#include "LightSceneNode.h"
#include "MeshSceneNode.h"
#include "Renderer.h"
#include "TextureManager.h"

static SceneData s_sceneData;

void Scene::init()
{
    auto mesh = std::make_shared<Mesh>("my/unused/model/path/model.glb");
    auto shader = std::make_shared<Shader>("assets/phong.vs", "assets/phong.fs");
    auto texture = TextureManager::loadTexture("assets/wall.png");

    auto sceneNode = std::make_unique<MeshSceneNode>(mesh, shader, texture);
    sceneNode->setPosition(glm::vec3(0, -10, 0));
    sceneNode->setScale(glm::vec3(200, 1, 200));
    s_sceneData.m_meshSceneNodes.emplace_back(std::move(sceneNode));

    auto sceneNode2 = std::make_unique<MeshSceneNode>(mesh, shader, texture);
    sceneNode2->setPosition(glm::vec3(30, 0, 30));
    s_sceneData.m_meshSceneNodes.emplace_back(std::move(sceneNode2));

    auto light1 = std::make_unique<LightSceneNode>(glm::vec3(80, 20, 50), glm::vec3(1, 0, 0), glm::vec3(-1, -1, 0));
    s_sceneData.m_lightSceneNodes.emplace_back(std::move(light1));
    auto light2 = std::make_unique<LightSceneNode>(glm::vec3(35, 15, 76), glm::vec3(0, 1, 0), glm::vec3(1, -1, -0.3));
    s_sceneData.m_lightSceneNodes.emplace_back(std::move(light2));
    auto light3 = std::make_unique<LightSceneNode>(glm::vec3(35, 15, 34), glm::vec3(0, 0, 1), glm::vec3(1, -1, -0.3));
    s_sceneData.m_lightSceneNodes.emplace_back(std::move(light3));
    auto light4 = std::make_unique<LightSceneNode>(glm::vec3(50, 15, 50), glm::vec3(1, 0, 1), glm::vec3(-0.3, -1, -1));
    s_sceneData.m_lightSceneNodes.emplace_back(std::move(light4));
}

void Scene::update()
{
    submitLights();
    submitMeshSceneNodes();
}

void Scene::submitLights()
{
    for (const auto& node : s_sceneData.m_lightSceneNodes)
    {
        Renderer::submitLight(node->prepareLight());
    }
}

void Scene::submitMeshSceneNodes()
{
    for (const auto& node : s_sceneData.m_meshSceneNodes)
    {
        Renderer::submitRenderable(node->prepareRenderable());
    }
}