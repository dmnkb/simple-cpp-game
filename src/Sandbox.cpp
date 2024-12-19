#include "Sandbox.h"
#include "pch.h"

#include "LightSceneNode.h"
#include "Mesh.h"
#include "MeshSceneNode.h"
#include "Scene.h"
#include "Shader.h"
#include "TextureManager.h"

void Sandbox::init()
{
    auto mesh = std::make_shared<Mesh>("my/unused/model/path/model.glb");
    auto shader = std::make_shared<Shader>("assets/phong.vs", "assets/phong.fs");
    auto texture = TextureManager::loadTexture("assets/wall.png");

    auto sceneNode = std::make_shared<MeshSceneNode>(mesh, shader, texture);
    sceneNode->setPosition(glm::vec3(0, -10, 0));
    sceneNode->setScale(glm::vec3(200, 1, 200));
    Scene::addMeshSceneNode(sceneNode);

    auto sceneNode2 = std::make_shared<MeshSceneNode>(mesh, shader, texture);
    sceneNode2->setPosition(glm::vec3(30, 0, 30));
    Scene::addMeshSceneNode(sceneNode2);

    auto light1 = std::make_shared<LightSceneNode>(glm::vec3(80, 20, 50), glm::vec3(1, 0, 0), glm::vec3(-1, -1, 0));
    Scene::addLightSceneNode(light1);
    auto light2 = std::make_shared<LightSceneNode>(glm::vec3(35, 15, 76), glm::vec3(0, 1, 0), glm::vec3(1, -1, -0.3));
    Scene::addLightSceneNode(light2);
    auto light3 = std::make_shared<LightSceneNode>(glm::vec3(35, 15, 34), glm::vec3(0, 0, 1), glm::vec3(1, -1, -0.3));
    Scene::addLightSceneNode(light3);
    auto light4 = std::make_shared<LightSceneNode>(glm::vec3(50, 15, 50), glm::vec3(1, 0, 1), glm::vec3(-0.3, -1, -1));
    Scene::addLightSceneNode(light4);
}

void Sandbox::update() {}