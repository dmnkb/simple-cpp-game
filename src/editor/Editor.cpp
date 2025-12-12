#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "assets/MeshLoader.h"
#include "core/Window.h"
#include "editor/Editor.h"
#include "editor/PanelFrametime.h"
#include "editor/PanelScene.h"
#include "editor/PanelSceneHierarchy.h"
#include "editor/PanelStats.h"
#include "editor/PanelStatsHighlights.h"
#include "renderer/DirectionalLight.h"
#include "renderer/Material.h"
#include "renderer/Shader.h"
#include "renderer/SpotLight.h"
#include "renderer/Texture.h"
#include "scene/Entity.h"

namespace Engine
{

Editor::Editor(const Ref<Scene>& activeScene)
{
    m_viewportCamController = CreateRef<FlySpectatorCtrl>();

    throwAwayDemoScene(activeScene);
}

void Editor::onUpdate(float fps, const Ref<Scene>& activeScene, const double deltaTime)
{
    m_viewportCamController->update(activeScene->getActiveCamera(), deltaTime);

    onImGuiRender(fps, activeScene, deltaTime);
}

void Editor::onImGuiRender(float fps, const Ref<Scene>& activeScene, const double deltaTime)
{
    PanelStats::render(fps);
    PanelFrametime::render();
    PanelSceneHierarchy::render(activeScene);
    // PanelScene::render(activeScene);
    // PanelStatsHighlights::render(fps, activeScene);
}

// TODO: remove once scene loading is in place
// TODO: consider the editor owning the scene
void Editor::throwAwayDemoScene(const Ref<Scene>& activeScene)
{
    auto loadedSubmeshes = MeshLoader::loadMeshFromFile("assets/models/ground/ground.gltf");

    if (!loadedSubmeshes)
    {
        std::println("[Editor] Failed to load model!");
        return;
    }

    int submeshIndex = 0;
    for (auto& submesh : *loadedSubmeshes)
    {
        std::println("Loaded submesh {} with {} vertices and {} indices.", submeshIndex, submesh.mesh->vertices.size(),
                     submesh.mesh->indices.size());

        // Create a new entity for each submesh
        auto entity = activeScene->createEntity("Ground_" + std::to_string(submeshIndex));

        auto& meshComp = entity.addComponent<MeshComponent>();
        meshComp.mesh = submesh.mesh;

        Ref<Material> material = CreateRef<Material>(Shader::getStandardShader());
        material->assignTexture(submesh.materialData.albedo, TextureType::Albedo);
        meshComp.material = material;

        submeshIndex++;
    }

    submeshIndex = (*loadedSubmeshes).size();
    for (auto& submesh : *loadedSubmeshes)
    {
        std::println("Loaded submesh {} with {} vertices and {} indices.", submeshIndex, submesh.mesh->vertices.size(),
                     submesh.mesh->indices.size());

        // Create a new entity for each submesh
        auto entity = activeScene->createEntity("Ground_" + std::to_string(submeshIndex));

        entity.getComponent<TransformComponent>().translate({0.0f, 5.0f, 0.0f});

        auto& meshComp = entity.addComponent<MeshComponent>();
        meshComp.mesh = submesh.mesh;

        Ref<Material> material = CreateRef<Material>(Shader::getStandardShader());
        material->assignTexture(submesh.materialData.albedo, TextureType::Albedo);
        meshComp.material = material;

        submeshIndex++;
    }

    // SpotLight::SpotLightProperties sp2{};
    // sp2.position = {-45, 10.0, 5.0};
    // sp2.direction = {1.8, 0, -.65};
    // sp2.colorIntensity = {1.0f, 1.0f, 1.0f, 25.0f};
    // activeScene->addSpotLight(CreateRef<SpotLight>(sp2));

    // PointLight::PointLightProperties p2{};
    // p2.position = {20, 5.0, 5};
    // p2.colorIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
    // activeScene->addPointLight(CreateRef<PointLight>(p2));

    // DirectionalLight::DirectionalLightProperties dl{};
    // dl.direction = {-1.0f, -1.0f, -1.0f};
    // dl.colorIntensity = {0.925f, 0.886f, 0.804f, 0.702f};
    // activeScene->setDirectionalLight(CreateRef<DirectionalLight>(dl));

    // // Managers / asset context
    // // TODO: Combine into AssetManager class?
    // MaterialManager materialManager;
    // ShaderManager shaderManager;
    // TextureManager textureManager;
    // AssetContext context(materialManager, shaderManager, textureManager);

    // const auto ground = Model("assets/models/ground/ground.gltf", context, {0.0f, 0.0f, 0.0f}, {5.0f, 1.0f, 5.0f});
    // activeScene->addModel(ground);
    // activeScene->addModel(Model("assets/models/cube/cube.gltf", context, {-20.0f, 5.0f, 0.0f}, {1.0f, 10.0f, 1.0f}));
    // activeScene->addModel(Model("assets/models/cube/cube.gltf", context, {0.0f, 5.0f, 0.0f}, {1.0f, 20.0f, 10.0f}));
    // activeScene->addModel(Model("assets/models/cube/cube.gltf", context, {20.0f, 5.0f, 0.0f}, {1.0f, 10.0f, 1.0f}));
}

} // namespace Engine