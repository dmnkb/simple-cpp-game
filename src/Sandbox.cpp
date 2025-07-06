#include "Sandbox.h"
#include "LightSceneNode.h"
#include "Material.h"
#include "Model.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "pch.h"

void Sandbox::init(Scene& scene)
{
    // auto mesh = CreateRef<Mesh>("my/unused/model/path/model.glb");
    // auto lightTexture = CreateRef<Texture>("assets/texture_08.png");
    // auto shader = CreateRef<Shader>("assets/phong.vs", "assets/phong.fs");

    // auto groundMaterial = CreateRef<Material>(shader);
    // groundMaterial->setDiffuseMap(lightTexture);
    // groundMaterial->setTextureRepeat(256);

    // auto groundNode = CreateRef<MeshSceneNode>(mesh, groundMaterial);
    // groundNode->setPosition(glm::vec3(0, -10, 0));
    // groundNode->setScale(glm::vec3(512, 1, 512));
    // Scene::addMeshSceneNode(groundNode);

    // auto oneMeterCube = CreateRef<MeshSceneNode>(mesh, groundMaterial);
    // oneMeterCube->setPosition(glm::vec3(0, 0, 0));
    // oneMeterCube->setScale(glm::vec3(1, 1, 1));
    // Scene::addMeshSceneNode(oneMeterCube);

    auto light1 =
        CreateRef<LightSceneNode>(glm::vec3(-10, 15, 50), glm::vec3(1, 0, 0), glm::vec3(0, -.5, -1), ELT_SPOT);
    scene.addLightSceneNode(light1);

    // auto light2 = CreateRef<LightSceneNode>(glm::vec3(0, 15, 50), glm::vec3(0, 1, 0), glm::vec3(0, -.5, -1),
    // ELT_SPOT); Scene::addLightSceneNode(light2);

    // auto light3 = CreateRef<LightSceneNode>(glm::vec3(10, 15, 50), glm::vec3(0, 0, 1), glm::vec3(0, -.5, -1),
    // ELT_SPOT); Scene::addLightSceneNode(light3);

    scene.addLightSceneNode(
        CreateRef<LightSceneNode>(glm::vec3(10, 15, 50), glm::vec3(0, 0, 1), glm::vec3(0, -.5, -1), ELT_SPOT));

    scene.addLightSceneNode(CreateRef<LightSceneNode>(glm::vec3(100.0f, 200.0f, 100.0f), // Position of the sun
                                                      glm::vec3(0.8f, 0.7f, 0.6f),       // Softer warm color
                                                      glm::vec3(-0.5f, -1.0f, -0.5f),    // Direction of sunlight
                                                      ELT_DIRECTIONAL                    // Type: Directional light
                                                      ));

    // load models
    // -----------

    // scene.addModel(Model("assets/models/sponza/glTF/Sponza.gltf"));

    MaterialManager materialManager;
    ShaderManager shaderManager;
    TextureManager textureManager;

    AssetContext context(materialManager, shaderManager, textureManager);

    scene.addModel(Model("assets/models/tree/scene.gltf", context, glm::vec3(-10.0, 0.0, 5.0)));
    scene.addModel(Model("assets/models/tree/scene.gltf", context, glm::vec3(10.0, 0.0, -2.0)));
    scene.addModel(Model("assets/models/tree/scene.gltf", context));

    scene.addModel(
        Model("assets/models/cube/cube.gltf", context, glm::vec3(0.0, -0.5, 0.0), glm::vec3(1000.0, 1, 1000.0)));
}

void Sandbox::update(double deltaTime) {}
