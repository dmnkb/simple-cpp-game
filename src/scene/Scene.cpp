#include "scene/Scene.h"
#include "core/Profiler.h"
#include "pch.h"
#include "scene/Entity.h"

namespace Engine
{

Scene::Scene()
{
    m_activeCamera = Camera(Camera::CameraProps{.fov = glm::radians(45.0f),
                                                .aspect = 16.f / 9.f, // TODO: change onViewportResize
                                                .near = 0.1f,
                                                .far = 1000.0f,
                                                .isMainCamera = true});
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
    RenderQueue renderQueue = {};

    auto group = m_registry.group<TransformComponent>(entt::get<MeshComponent>);

    for (auto entity : group)
    {
        const auto& [transformComp, meshComp] = group.get<TransformComponent, MeshComponent>(entity);
        const Ref<Mesh>& mesh = meshComp.mesh;
        if (!mesh) continue;

        const auto& instanceTransform = transformComp.getTransform();

        for (uint32_t i = 0; i < mesh->submeshes.size(); ++i)
        {
            Ref<Material> material;
            if (i > meshComp.materials.size())
            {
                std::cerr << "Warning: Mesh submesh " << i << " exceeds material count. Skipping." << std::endl;
                continue;
            }

            if (!meshComp.materials[i])
            {
                std::cerr << "Warning: Mesh submesh " << i << " has no assigned material. Skipping." << std::endl;
                continue;
            }

            material = meshComp.materials[i];

            if (!material) continue;

            SubmeshKey key{mesh, i};
            renderQueue[material][key].emplace_back(instanceTransform);
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

void Scene::setActiveCamera(const Camera& camera)
{
    m_activeCamera = camera;
}

Camera& Scene::getActiveCamera()
{
    return m_activeCamera;
}

} // namespace Engine