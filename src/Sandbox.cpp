#include "Sandbox.h"
#include "LightSceneNode.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshSceneNode.h"
#include "Scene.h"
#include "Shader.h"
#include "TextureManager.h"
#include "pch.h"

void Sandbox::init()
{
    auto mesh = CreateRef<Mesh>("my/unused/model/path/model.glb");
    auto texture1 = TextureManager::loadTextureFromFile("assets/texture_01.png");
    auto texture2 = TextureManager::loadTextureFromFile("assets/texture_02.png");
    auto shader = CreateRef<Shader>("assets/phong.vs", "assets/phong.fs");

    auto material1 = CreateRef<Material>(shader);
    material1->setColorTexture(texture1);
    material1->setColorTextureScale(16);

    auto sceneNode = CreateRef<MeshSceneNode>(mesh, material1);
    sceneNode->setPosition(glm::vec3(0, -1, 0));
    sceneNode->setScale(glm::vec3(100, 1, 100));
    Scene::addMeshSceneNode(sceneNode);

    auto material2 = CreateRef<Material>(shader);
    material2->setColorTexture(texture2);

    auto sceneNode2 = CreateRef<MeshSceneNode>(mesh, material2);
    sceneNode2->setName("Floating Cube");
    sceneNode2->setPosition({0, 10, 0});
    Scene::addMeshSceneNode(sceneNode2);

    auto light1 = CreateRef<LightSceneNode>(glm::vec3(5, 15, 0), glm::vec3(1, 0, 0), glm::vec3(-1, -1, 0), ELT_SPOT);
    Scene::addLightSceneNode(light1);

    auto light2 = CreateRef<LightSceneNode>(glm::vec3(-3, 12, 4), glm::vec3(0, 1, 0), glm::vec3(1, -1, -1));
    Scene::addLightSceneNode(light2);
}

static float s_increment = 0.0f;

void Sandbox::update(double deltaTime)
{
    s_increment += 0.5 * deltaTime;

    const auto testNode = Scene::getByName("Floating Cube");
    if (testNode && Is(Ref<MeshSceneNode>, *testNode))
    {
        const auto node = Get(Ref<MeshSceneNode>, *testNode);
        node->setPosition(glm::vec3(0, sin(s_increment) * 4 + 4, 0));
    }
}