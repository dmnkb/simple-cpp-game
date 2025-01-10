#include "Sandbox.h"
#include "LightSceneNode.h"
#include "Mesh.h"
#include "MeshSceneNode.h"
#include "Scene.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"

void Sandbox::init()
{
    auto mesh = CreateRef<Mesh>("my/unused/model/path/model.glb");
    auto shader = CreateRef<Shader>("assets/phong.vs", "assets/phong.fs");
    auto texture = TextureManager::loadTextureFromFile("assets/wall.png");

    auto sceneNode = CreateRef<MeshSceneNode>(mesh, shader, texture);
    sceneNode->setPosition(glm::vec3(0, -10, 0));
    sceneNode->setScale(glm::vec3(100, 1, 100));
    Scene::addMeshSceneNode(sceneNode);

    auto sceneNode2 = CreateRef<MeshSceneNode>(mesh, shader, texture);
    sceneNode2->setName("Floating Cube");
    Scene::addMeshSceneNode(sceneNode2);

    auto light1 = CreateRef<LightSceneNode>(glm::vec3(10, 10, 0), glm::vec3(1, 0, 0), glm::vec3(-1, -2, 0));
    Scene::addLightSceneNode(light1);

    auto light2 = CreateRef<LightSceneNode>(glm::vec3(-3, 2, 4), glm::vec3(0, 1, 0), glm::vec3(1, -1, -0.3));
    light2->setName("Green Light");
    Scene::addLightSceneNode(light2);

    auto light3 = CreateRef<LightSceneNode>(glm::vec3(2, 3, -3), glm::vec3(0, 0, 1), glm::vec3(1, -1, -0.3));
    Scene::addLightSceneNode(light3);

    auto light4 = CreateRef<LightSceneNode>(glm::vec3(-4, 2, -4), glm::vec3(1, 0, 1), glm::vec3(-0.3, -1, -1));
    Scene::addLightSceneNode(light4);
}

static float s_increment = 0.0f;

void Sandbox::update(double deltaTime)
{
    s_increment += 0.5 * deltaTime;

    const auto testNode = Scene::getByName("Floating Cube");
    if (testNode && Is(Ref<MeshSceneNode>, *testNode))
    {
        const auto node = Get(Ref<MeshSceneNode>, *testNode);
        node->setPosition(glm::vec3(0, sin(s_increment) * 2 - 4, 0));
    }

    const auto greenLight = Scene::getByName("Green Light");
    if (greenLight && Is(Ref<LightSceneNode>, *greenLight))
    {
        const auto light = Get(Ref<LightSceneNode>, *greenLight);
        light->setPosition(glm::vec3(sin(s_increment) * 3 - 10, 2, cos(s_increment) * 3));
    }
}