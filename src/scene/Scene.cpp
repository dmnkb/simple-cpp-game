#include "scene/Scene.h"
#include "core/Profiler.h"
#include "pch.h"

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

void Scene::addModel(const Model& model)
{
    m_models.push_back(model);
    m_renderablesDirty = true;
}

void Scene::addSpotLight(const Ref<SpotLight>& light)
{
    m_spotLights.push_back(light);
}

void Scene::addPointLight(const Ref<PointLight>& light)
{
    m_pointLights.push_back(light);
}

void Scene::setDirectionalLight(const Ref<DirectionalLight>& light)
{
    m_directionalLight = light;
}

// TODO: Sort transparent renderables back-to-front
RenderQueue Scene::getRenderQueue(const std::string& passName)
{
    if (!m_renderablesDirty)
    {
        return m_cachedRenderQueue;
    }

    RenderQueue renderQueue = {};

    // Group renderables by their name to ensure identical meshes are processed together
    std::unordered_map<std::string, std::vector<Renderable>> sortedRenderables = {};
    for (const auto& model : m_models)
    {
        for (const auto& renderable : model.renderables)
        {
            sortedRenderables[renderable.name].push_back(renderable);
        }
    }

    // Process each group of renderables, collecting transforms and associating materials and meshes
    for (const auto& [meshName, renderables] : sortedRenderables)
    {
        std::vector<glm::mat4> transforms = {};
        Ref<Material> material = nullptr;
        Ref<Mesh> mesh = nullptr;
        for (const auto& renderable : renderables)
        {
            transforms.push_back(renderable.transform);
            if (!material)
                material = renderable.material;

            if (!mesh)
                mesh = renderable.mesh;
        }
        renderQueue[material][mesh] = transforms;
    }

    m_renderablesDirty = false;
    m_cachedRenderQueue = renderQueue;

    return renderQueue;
}

std::vector<Ref<SpotLight>> Scene::getSpotLights() const
{
    return m_spotLights;
}

std::vector<Ref<PointLight>> Scene::getPointLights() const
{
    return m_pointLights;
}

Ref<DirectionalLight> Scene::getDirectionalLight() const
{
    return m_directionalLight;
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