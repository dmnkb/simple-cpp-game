#include "Scene.h"
#include "Renderer.h"

Scene::Scene()
{
    m_lights.push_back({
        glm::vec3(80, 20, 50), // Position
        0.0f,                  // Padding
        glm::vec3(1, 1, 1),    // Color: red
        0.0f,                  // Padding
        glm::vec3(-1, -1, 0),  // Direction: pointing downward
        0.0f,                  // Padding
        ELightType::SPOT,      // Light type: spot
        20.0f,                 // Inner cone angle in radians
        30.0f,                 // Outer cone angle in radians
        0.0f,                  // Padding
    });

    m_lights.push_back({
        glm::vec3(35, 15, 76),  // Position
        0.0f,                   // Padding
        glm::vec3(0, 1, 0),     // Color: green
        0.0f,                   // Padding
        glm::vec3(-1, -1, -.5), // Direction: downward
        0.0f,                   // Padding
        ELightType::SPOT,       // Light type: spot
        20.0f,                  // Inner cone angle in radians
        30.0f,                  // Outer cone angle in radians
        0.0f,                   // Padding
    });

    m_lights.push_back({
        glm::vec3(35, 15, 34),  // Position
        0.0f,                   // Padding
        glm::vec3(0, 0, 1),     // Color: blue
        0.0f,                   // Padding
        glm::vec3(1, -1, -0.3), // Slightly tilted downward
        0.0f,                   // Padding
        ELightType::SPOT,       // Light type: spot
        20.0f,                  // Inner cone angle in radians
        30.0f,                  // Outer cone angle in radians
        0.0f,                   // Padding
    });

    m_lights.push_back({
        glm::vec3(50, 15, 50),   // Position
        0.0f,                    // Padding
        glm::vec3(1, 0, 1),      // Color: purple
        0.0f,                    // Padding
        glm::vec3(-0.3, -1, -1), // Tilted downward
        0.0f,                    // Padding
        ELightType::SPOT,        // Light type: spot
        25.0f,                   // Inner cone angle in radians
        45.0f,                   // Outer cone angle in radians
        0.0f,                    // Padding
    });
}

void Scene::update()
{
    Renderer::submitLights(m_lights);
}