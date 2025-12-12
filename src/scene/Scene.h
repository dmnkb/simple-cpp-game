#pragma once

#include <entt/entt.hpp>

#include "assets/MeshLoader.h"
#include "renderer/Camera.h"
#include "renderer/DirectionalLight.h"
#include "scene/Components.h"

namespace Engine
{

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

class Entity;

class Scene
{
  public:
    Scene();
    ~Scene();

    // Entities
    Entity createEntity(const std::string& name = std::string());

    void destroyEntity(entt::entity entity);

    // Update
    // void onUpdate(float deltaTime);

    // Rendering
    RenderQueue getRenderQueue(const std::string& passName);

    template <LightComponent LightComp>
    auto getLights()
    {
        return m_registry.view<TransformComponent, LightComp>();
    }

    // Lighting
    void setAmbientLightColor(const glm::vec4& color);
    glm::vec4 getAmbientLightColor() const;

    // Camera
    void setActiveCamera(const Camera& camera);
    Camera& getActiveCamera();

  private:
    entt::registry m_registry;

    glm::vec4 m_ambientLightColor = {0.302f, 0.329f, 0.357f, 1.0f};
    glm::vec4 m_sunLightColor = {0.925f, 0.886f, 0.804f, 0.702f};

    Camera m_activeCamera;

    friend class Entity;
};

} // namespace Engine
