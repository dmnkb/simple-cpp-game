#include "scene/Scene.h"
#include "core/Profiler.h"
#include "pch.h"
#include "scene/Entity.h"

namespace Engine
{

Scene::Scene()
{
    Camera::CameraProps props = {};
    props.fov = glm::radians(45.0f);
    props.aspect = 16.f / 9.f;
    props.near = 0.1f;
    props.far = 1000.0f;
    props.isMainCamera = true;

    m_activeCamera = Camera(props);
}

Scene::~Scene() {}

Entity Scene::createEntity(std::string const& name)
{
    Entity entity = {m_registry.create(), this};
    entity.addComponent<TransformComponent>();
    auto& tag = entity.addComponent<TagComponent>();
    tag.tag = name.empty() ? "Entity" : name;

    return entity;
}

void Scene::destroyEntity(entt::entity entity)
{
    m_registry.destroy(entity);
}

// TODO: Sort transparent renderables back-to-front
// TODO: Maybe consider submitting instanced meshes to the rendererAPI directly so we don't have to loop twice
RenderQueue Scene::getRenderQueue(const std::string& passName)
{
    RenderQueue renderQueue{};

    auto group = m_registry.group<TransformComponent>(entt::get<MeshComponent>);

    for (auto entity : group)
    {
        const auto& [transformComp, meshComp] = group.get<TransformComponent, MeshComponent>(entity);

        const Ref<Mesh>& mesh = meshComp.mesh;
        if (!mesh) continue;

        const glm::mat4 instanceTransform = transformComp.getTransform();

        for (uint32_t i = 0; i < mesh->submeshes.size(); ++i)
        {
            Ref<Material> material = nullptr;

            // 1) optional override
            if (i < meshComp.overrideMaterials.size())
            {
                material = meshComp.overrideMaterials[i];
            }

            // 2) fallback to mesh default slot
            if (!material && i < mesh->defaultMaterialSlots.size())
            {
                material = mesh->defaultMaterialSlots[i];
            }

            // 3) no material assigned
            if (!material) continue;

            SubmeshKey key{mesh, i};
            renderQueue[material->metadata.uuid][key].emplace_back(instanceTransform);
        }
    }

    return renderQueue;
}

void Scene::setAmbientLightColor(const glm::vec4& color)
{
    m_ambientLightColor = color;
}

glm::vec4 Scene::getAmbientLightColor() const
{
    return m_ambientLightColor;
}

void Scene::setSunLightColor(const glm::vec4& color)
{
    m_sunLightColor = color;
}

glm::vec4 Scene::getSunLightColor() const
{
    return m_sunLightColor;
}

void Scene::setActiveCamera(const Camera& camera)
{
    m_activeCamera = camera;
}

Camera& Scene::getActiveCamera()
{
    return m_activeCamera;
}

} // namespace Engine