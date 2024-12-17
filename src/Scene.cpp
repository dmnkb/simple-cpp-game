#include "Scene.h"
#include "MeshSceneNode.h"
#include "Renderer.h"
#include "TextureManager.h"

static SceneData s_sceneData;

void Scene::init()
{
    auto mesh = std::make_shared<Mesh>("my/unused/model/path/model.glb");
    auto shader = std::make_shared<Shader>("assets/phong.vs", "assets/phong.fs");
    auto texture = TextureManager::loadTexture("assets/wall.png");

    auto sceneNode = std::make_unique<MeshSceneNode>(mesh, "MeshSceneNode Name", nullptr, shader, texture);
    sceneNode->setPosition(glm::vec3(0, -10, 0));
    sceneNode->setScale(glm::vec3(200, 1, 200));

    auto sceneNode2 = std::make_unique<MeshSceneNode>(mesh, "MeshSceneNode Name", nullptr, shader, texture);
    sceneNode2->setPosition(glm::vec3(3, 0, 0));

    s_sceneData.m_meshSceneNodes.emplace_back(std::move(sceneNode));
    s_sceneData.m_meshSceneNodes.emplace_back(std::move(sceneNode2));

    s_sceneData.m_lights.push_back({
        glm::vec3(80, 20, 50), // Position
        0.0f,                  // Padding
        glm::vec3(1, 0, 0),    // Color: red
        0.0f,                  // Padding
        glm::vec3(-1, -1, 0),  // Direction: SPOTing downward
        0.0f,                  // Padding
        ELightType::SPOT,      // Light type: SPOT
        20.0f,                 // Inner cone angle in radians
        30.0f,                 // Outer cone angle in radians
        0.0f,                  // Padding
    });

    s_sceneData.m_lights.push_back({
        glm::vec3(35, 15, 76),  // Position
        0.0f,                   // Padding
        glm::vec3(0, 1, 0),     // Color: green
        0.0f,                   // Padding
        glm::vec3(-1, -1, -.5), // Direction: downward
        0.0f,                   // Padding
        ELightType::SPOT,       // Light type: SPOT
        20.0f,                  // Inner cone angle in radians
        30.0f,                  // Outer cone angle in radians
        0.0f,                   // Padding
    });

    s_sceneData.m_lights.push_back({
        glm::vec3(35, 15, 34),  // Position
        0.0f,                   // Padding
        glm::vec3(0, 0, 1),     // Color: blue
        0.0f,                   // Padding
        glm::vec3(1, -1, -0.3), // Slightly tilted downward
        0.0f,                   // Padding
        ELightType::SPOT,       // Light type: SPOT
        20.0f,                  // Inner cone angle in radians
        30.0f,                  // Outer cone angle in radians
        0.0f,                   // Padding
    });

    s_sceneData.m_lights.push_back({
        glm::vec3(50, 15, 50),   // Position
        0.0f,                    // Padding
        glm::vec3(1, 0, 1),      // Color: purple
        0.0f,                    // Padding
        glm::vec3(-0.3, -1, -1), // Tilted downward
        0.0f,                    // Padding
        ELightType::SPOT,        // Light type: SPOT
        25.0f,                   // Inner cone angle in radians
        45.0f,                   // Outer cone angle in radians
        0.0f,                    // Padding
    });
}

void Scene::update()
{
    submitLights();
    submitMeshSceneNodes();
}

void Scene::submitLights()
{
    Renderer::submitLights(s_sceneData.m_lights);
}

void Scene::submitMeshSceneNodes()
{
    for (const auto& node : s_sceneData.m_meshSceneNodes)
    {
        Renderer::submitRenderable(node->prepareRenderable());
    }
}