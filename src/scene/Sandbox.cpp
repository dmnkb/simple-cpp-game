#include "Sandbox.h"
#include "pch.h"
#include "renderer/Material.h"
#include "renderer/Model.h"
#include "renderer/Shader.h"
#include "renderer/SpotLight.h"
#include "renderer/Texture.h"
#include "scene/Scene.h"

namespace Engine
{

void Sandbox::init(Scene& scene)
{

    scene.addSpotLight(CreateRef<SpotLight>(SpotLight::SpotLightProperties{.position = {5.0f, 5.0f, 5.0f},
                                                                           .direction = {
                                                                               -1,
                                                                               -1,
                                                                               -1,
                                                                           }}));
    scene.addSpotLight(CreateRef<SpotLight>(SpotLight::SpotLightProperties{.position = {15.0f, 5.0f, 5.0f},
                                                                           .direction = {
                                                                               -1,
                                                                               -1,
                                                                               -1,
                                                                           }}));

    // Models

    MaterialManager materialManager;
    ShaderManager shaderManager;
    TextureManager textureManager;

    AssetContext context(materialManager, shaderManager, textureManager);

    scene.addModel(Model("assets/models/tree/scene.gltf", context, {10.0, 0.0, -10.0}));
    scene.addModel(Model("assets/models/cube/cube.gltf", context, {0.0, 0.0, 0.0}));
    scene.addModel(Model("assets/models/cube/cube.gltf", context, {10.0, 0.0, 0.0}));
    scene.addModel(Model("assets/models/ground/ground.gltf", context, {0.0, -1, 0.0}, {10.0, 1.0, 10.0}));
}

void Sandbox::update(double deltaTime)
{
    static double elapsedTime = 0.0;
    elapsedTime += deltaTime;

    float radius = 80.0f;
    float height = 40.0f;
    float speed = 0.05f; // radians per second

    // Circular horizontal movement
    float x = radius * cos(elapsedTime * speed);
    float z = radius * sin(elapsedTime * speed);
    float y = height + sin(elapsedTime * 0.8f) * 5.0f; // gentle vertical bobbing

    glm::vec3 newPosition = glm::vec3(x, y, z);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // point light looks at
    glm::vec3 direction = glm::normalize(target - newPosition);

    // m_movingLight->setPosition(newPosition);
    // m_movingLight->setLookAt({0, 0, 0});
}

} // namespace Engine