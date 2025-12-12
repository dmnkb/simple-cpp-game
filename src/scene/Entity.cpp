#include "scene/Entity.h"

#include "pch.h"

namespace Engine
{

Entity::Entity(entt::entity handle, Scene* scene) : m_handle(handle), m_scene(scene) {}

} // namespace Engine
