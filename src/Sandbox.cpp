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
    // scene.addLightSceneNode(
    //     CreateRef<LightSceneNode>(glm::vec3(-10, 15, 50), glm::vec3(1, 0, 0), glm::vec3(0, -.5, -1), ELT_SPOT));

    // scene.addLightSceneNode(
    //     CreateRef<LightSceneNode>(glm::vec3(0, 15, 50), glm::vec3(0, 1, 0), glm::vec3(0, -.5, -1), ELT_SPOT));

    // scene.addLightSceneNode(
    //     CreateRef<LightSceneNode>(glm::vec3(10, 15, 50), glm::vec3(0, 0, 1), glm::vec3(0, -.5, -1), ELT_SPOT));

    // scene.addLightSceneNode(
    //     CreateRef<LightSceneNode>(glm::vec3(10, 15, 50), glm::vec3(0, 0, 1), glm::vec3(0, -.5, -1), ELT_SPOT));

    scene.addLightSceneNode(CreateRef<LightSceneNode>(glm::vec3(100.0f, 200.0f, 100.0f), // Position of the sun
                                                      glm::vec3(0.8f, 0.7f, 0.6f),       // Softer warm color
                                                      glm::vec3(-0.5f, -1.0f, -0.5f),    // Direction of sunlight
                                                      ELT_DIRECTIONAL                    // Type: Directional light
                                                      ));

    // Models

    MaterialManager materialManager;
    ShaderManager shaderManager;
    TextureManager textureManager;

    AssetContext context(materialManager, shaderManager, textureManager);

    scene.addModel(Model("assets/models/tree/scene.gltf", context));

    scene.addModel(Model("assets/models/cube/cube.gltf", context, glm::vec3(0.0, -0.5, 0.0), glm::vec3(10.0, 1, 10.0)));
}

void Sandbox::update(double deltaTime) {}
