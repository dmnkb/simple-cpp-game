#include "Scene.h"
#include "MeshSceneNode.h"
#include "Renderer.h"
#include "TextureManager.h"

Scene::Scene()
{
    auto mesh = std::make_shared<Mesh>("my/unused/model/path/model.glb");
    auto shader = std::make_shared<Shader>("assets/phong.vs", "assets/phong.fs");
    auto texture = TextureManager::loadTexture("assets/wall.png");

    m_meshSceneNodes.emplace_back(
        std::make_unique<MeshSceneNode>(mesh, "MeshSceneNode Name", nullptr, shader, texture));

    // m_lights.push_back({
    //     glm::vec3(80, 20, 50), // Position
    //     0.0f,                  // Padding
    //     glm::vec3(1, 1, 1),    // Color: red
    //     0.0f,                  // Padding
    //     glm::vec3(-1, -1, 0),  // Direction: pointing downward
    //     0.0f,                  // Padding
    //     ELightType::SPOT,      // Light type: spot
    //     20.0f,                 // Inner cone angle in radians
    //     30.0f,                 // Outer cone angle in radians
    //     0.0f,                  // Padding
    // });
}

void Scene::update()
{
    submitLights();
    submitMeshSceneNodes();
}

void Scene::submitLights()
{
    Renderer::submitLights(m_lights);
}

void Scene::submitMeshSceneNodes()
{
    for (const auto& node : m_meshSceneNodes)
    {
        Renderer::submitRenderable(node->prepareRenderable());
    }
}