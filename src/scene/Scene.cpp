#include "Scene.h"
#include "core/Profiler.h"
#include "pch.h"

namespace Engine
{

void Scene::init(const Camera::CameraProps& cameraProps)
{
    m_DefaultCamera = CreateRef<Camera>(cameraProps);
    m_DefaultCamera->isMainCamera();
    setActiveCamera(m_DefaultCamera);
}

void Scene::addModel(const Model& model)
{
    m_Models.push_back(model);
    m_renderablesDirty = true;
}

void Scene::addSpotLight(const Ref<SpotLight>& light)
{
    m_SpotLights.push_back(light);
}

void Scene::addPointLight(const Ref<PointLight>& light)
{
    m_PointLights.push_back(light);
}

void Scene::setDirectionalLight(const Ref<DirectionalLight>& light)
{
    m_DirectionalLight = light;
}

// TODO: Sort transparent renderables back-to-front
RenderQueue Scene::getRenderQueue(const std::string& passName)
{
    Engine::Profiler::beginRegion("Prepare Render Queue (" + passName + ")");

    if (!m_renderablesDirty)
    {
        Engine::Profiler::endRegion("Prepare Render Queue (" + passName + ")");
        return m_cachedRenderQueue;
    }

    RenderQueue renderQueue = {};

    // Group renderables by their name to ensure identical meshes are processed together
    std::unordered_map<std::string, std::vector<Renderable>> sortedRenderables = {};
    for (const auto& model : m_Models)
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

    Engine::Profiler::endRegion("Prepare Render Queue (" + passName + ")");
    return renderQueue;
}

std::vector<Ref<SpotLight>> Scene::getSpotLights() const
{
    return m_SpotLights;
}

std::vector<Ref<PointLight>> Scene::getPointLights() const
{
    return m_PointLights;
}

void Scene::setActiveCamera(const Ref<Camera>& camera)
{
    m_ActiveCamera = camera;
}

Ref<Camera> Scene::getActiveCamera() const
{
    return m_ActiveCamera;
}

Ref<Camera> Scene::getDefaultCamera() const
{
    return m_DefaultCamera;
}

} // namespace Engine