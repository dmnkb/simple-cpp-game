#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "assets/AssetManager.h"
#include "assets/AssetRegistry.h"
#include "assets/MeshLoader.h"
#include "core/Window.h"
#include "editor/Editor.h"
#include "editor/PanelAssets.h"
#include "editor/PanelComponents.h"
#include "editor/PanelMaterial.h"
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

    onLoad();
    throwAwayDemoScene(activeScene);
}

void Editor::onLoad() {}

void Editor::onUpdate(float fps, const Ref<Scene>& activeScene, const double deltaTime,
                      std::function<void(UUID)> onSelectScene)
{
    if (activeScene) m_viewportCamController->update(activeScene->getActiveCamera(), deltaTime);

    onImGuiRender(fps, activeScene, deltaTime, onSelectScene);
}

void Editor::onImGuiRender(float fps, const Ref<Scene>& activeScene, const double deltaTime,
                           std::function<void(UUID)> onSelectScene)
{
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    PanelStats::render(fps);
    PanelSceneHierarchy::render(activeScene, onSelectScene);
    PanelComponents::render(activeScene);
    PanelAssets::render();
    PanelMaterial::render();
    // PanelScene::render(activeScene);
    // PanelStatsHighlights::render(fps, activeScene);
}

// TODO: remove once scene loading is in place
void Editor::throwAwayDemoScene(const Ref<Scene>& activeScene)
{

    // auto groundMesh = AssetManager::getOrImport<Mesh>(
    //     AssetRegistry::findOrRegisterAsset(AssetType::Mesh, "assets/models/ground/ground.gltf", "Ground Mesh"));

    // if (groundMesh)
    // {

    //     auto entity = activeScene->createEntity("Ground");
    //     auto& meshComp = entity.addComponent<MeshComponent>();
    //     meshComp.mesh = groundMesh;

    //     for (const auto& matData : groundMesh->materials)
    //     {
    //         // TODO: Would happen via editor
    //         Ref<Material> material = AssetManager::getOrImport<Material>(m_defaultMaterialId);
    //         material->assignTexture(matData.albedo, TextureType::Albedo);
    //         meshComp.materials.push_back(material);
    //         meshComp.materialSlotNames.push_back(matData.materialSlotName);
    //     }
    // }

    // auto modelData = MeshLoader::loadMeshFromFile("./assets/models/sponza/glTF/Sponza.gltf");
    // if (modelData)
    // {
    //     auto entity = activeScene->createEntity("Sponza");
    //     entity.getComponent<TransformComponent>().translation = {0.0f, 0.0f, 0.0f};

    //     auto& meshComp = entity.addComponent<MeshComponent>();
    //     meshComp.mesh = modelData->mesh;

    //     for (const auto& matData : modelData->materials)
    //     {
    //         Ref<Material> material = CreateRef<Material>(Shader::getStandardShader());
    //         material->assignTexture(matData.albedo, TextureType::Albedo);
    //         material->assignTexture(matData.normal, TextureType::Normal);
    //         material->assignTexture(matData.roughness, TextureType::Roughness);
    //         material->assignTexture(matData.metallic, TextureType::Metallic);
    //         material->assignTexture(matData.ao, TextureType::AO);
    //         meshComp.materials.push_back(material);
    //     }
    // }

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