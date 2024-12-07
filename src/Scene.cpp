#include "Scene.h"
#include "Renderer.h"

Scene::Scene()
{
    m_lights.push_back({glm::vec3(80, 5, 50), 0.0f, glm::vec3(1, 0, 0), 0.0f});
    m_lights.push_back({glm::vec3(35, 5, 76), 0.0f, glm::vec3(0, 1, 0), 0.0f});
    m_lights.push_back({glm::vec3(35, 5, 34), 0.0f, glm::vec3(0, 0, 1), 0.0f});
    m_lights.push_back({glm::vec3(50, 5, 50), 0.0f, glm::vec3(1, 0, 1), 0.0f});
}

void Scene::update()
{
    Renderer::submitLights(m_lights);
}