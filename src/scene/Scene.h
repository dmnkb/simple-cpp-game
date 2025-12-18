#pragma once

#include <entt/entt.hpp>

#include "assets/MeshLoader.h"
#include "renderer/Camera.h"
#include "renderer/DirectionalLight.h"
#include "scene/Components.h"

namespace Engine
{

struct SubmeshKey
{
    Ref<Mesh> mesh;
    uint32_t submeshIndex;

    bool operator==(const SubmeshKey& other) const
    {
        return mesh == other.mesh && submeshIndex == other.submeshIndex;
    }
};

} // namespace Engine

namespace std
{
template <>
struct hash<Engine::SubmeshKey>
{
    size_t operator()(const Engine::SubmeshKey& key) const
    {
        size_t h1 = hash<Engine::Ref<Engine::Mesh>>()(key.mesh);
        size_t h2 = hash<uint32_t>()(key.submeshIndex);
        return h1 ^ (h2 << 1);
    }
};
} // namespace std

namespace Engine
{

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<SubmeshKey, std::vector<glm::mat4>>>;

class Entity;

class Scene
{
  public:
    Scene();
    ~Scene();

    // Entities
    Entity createEntity(const std::string& name = std::string());

    void destroyEntity(entt::entity entity);

    template <typename Func>
    void forEachHierarchyEntity(Func&& fn)
    {
        auto view = m_registry.view<TagComponent>();
        for (auto e : view)
            fn(e);
    }

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
