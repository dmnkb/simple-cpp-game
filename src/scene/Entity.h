#pragma once

#include <entt/entt.hpp>

#include "scene/Components.h"
#include "scene/Scene.h"

namespace Engine
{

class Entity
{
  public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene);
    Entity(const Entity& other) = default;

    template <typename T, typename... Args>
    T& addComponent(Args&&... args)
    {
        assert(!hasComponent<T>() && "Entity already has component!");
        return m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    template <typename T>
    T& getComponent()
    {
        assert(hasComponent<T>() && "Entity does not have component!");
        return m_scene->m_registry.get<T>(m_handle);
    }

    template <typename T>
    bool hasComponent() const
    {
        return m_scene->m_registry.all_of<T>(m_handle);
    }

    template <typename T>
    void removeComponent()
    {
        assert(hasComponent<T>() && "Entity does not have component!");
        m_scene->m_registry.remove<T>(m_handle);
    }

    inline glm::mat4 getTransformMatrix() { return getComponent<TransformComponent>(); }

    operator bool() const { return m_handle != entt::null; }

  private:
    entt::entity m_handle{entt::null};
    Scene* m_scene = nullptr;
};

} // namespace Engine
