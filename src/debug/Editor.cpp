#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Editor.h"
#include "PanelFrametime.h"
#include "PanelScene.h"
#include "PanelSceneHierarchy.h"
#include "PanelStats.h"
#include "PanelStatsHighlights.h"
#include "core/Window.h"
#include "renderer/DirectionalLight.h"
#include "renderer/Material.h"
#include "renderer/Model.h"
#include "renderer/PointLight.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "renderer/SpotLight.h"
#include "renderer/Texture.h"

namespace Engine
{

Editor::Editor(const Ref<Scene>& activeScene)
{
    m_viewportCamController = CreateRef<Player>();

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
    PanelScene::render(activeScene);
    PanelStatsHighlights::render(fps, activeScene);
}

// TODO: remove once scene loading is in place
void Editor::throwAwayDemoScene(const Ref<Scene>& activeScene)
{
    SpotLight::SpotLightProperties sp{};
    sp.position = {-20, 5.0, 15.0};
    sp.direction = {0, 0, -1.0};
    sp.colorIntensity = {1.0f, 1.0f, 1.0f, 5.0f};
    activeScene->addSpotLight(CreateRef<SpotLight>(sp));

    PointLight::PointLightProperties p1{};
    p1.position = {0, 5.0, 5};
    p1.colorIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
    activeScene->addPointLight(CreateRef<PointLight>(p1));

    PointLight::PointLightProperties p2{};
    p2.position = {20, 5.0, 5};
    p2.colorIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
    activeScene->addPointLight(CreateRef<PointLight>(p2));

    DirectionalLight::DirectionalLightProperties dl{};
    dl.direction = {-1.0f, -1.0f, -1.0f};
    dl.colorIntensity = {0.925f, 0.886f, 0.804f, 0.702f};
    activeScene->setDirectionalLight(CreateRef<DirectionalLight>(dl));

    // Managers / asset context
    // TODO: Combine into AssetManager class?
    MaterialManager materialManager;
    ShaderManager shaderManager;
    TextureManager textureManager;
    AssetContext context(materialManager, shaderManager, textureManager);

    const auto ground = Model("assets/models/ground/ground.gltf", context, {0.0f, 0.0f, 0.0f}, {5.0f, 1.0f, 5.0f});
    activeScene->addModel(ground);
    activeScene->addModel(Model("assets/models/cube/cube.gltf", context, {-20.0f, 5.0f, 0.0f}, {1.0f, 10.0f, 1.0f}));
    activeScene->addModel(Model("assets/models/cube/cube.gltf", context, {0.0f, 5.0f, 0.0f}, {1.0f, 20.0f, 10.0f}));
    activeScene->addModel(Model("assets/models/cube/cube.gltf", context, {20.0f, 5.0f, 0.0f}, {1.0f, 10.0f, 1.0f}));
}

} // namespace Engine