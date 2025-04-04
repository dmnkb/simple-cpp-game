#include "Scene.h"

void Scene::init(const CameraProps& cameraProps)
{
    m_DefaultCamera = CreateRef<Camera>(cameraProps);
    setActiveCamera(m_DefaultCamera);
}

void Scene::addModel(const Model& model)
{
    m_Models.push_back(model);
}

void Scene::addLightSceneNode(const Ref<LightSceneNode>& node)
{
    m_LightSceneNodes.push_back(node);
}

// TODO: Sort renderables back-to-front
RenderQueue Scene::getRenderQueue()
{
    RenderQueue renderQueue = {};

    // Group renderables by their name to ensure identical meshes are processed together
    std::unordered_map<std::string, std::vector<Ref<Renderable>>> sortedRenderables = {};
    for (const auto& model : m_Models)
    {
        for (const auto& renderable : model.renderables)
        {
            sortedRenderables[renderable->name].push_back(renderable);
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
            transforms.push_back(renderable->transform);
            if (!material)
                material = renderable->material;

            if (!mesh)
                mesh = renderable->mesh;
        }
        renderQueue[material][mesh] = transforms;
    }

    return renderQueue;
}

std::vector<Ref<LightSceneNode>> Scene::getLightSceneNodes()
{
    return m_LightSceneNodes;
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