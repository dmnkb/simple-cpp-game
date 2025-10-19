#include "Sandbox.h"
#include "glm/gtx/string_cast.hpp"
#include "pch.h"
#include "renderer/Material.h"
#include "renderer/Model.h"
#include "renderer/PointLight.h"
#include "renderer/Shader.h"
#include "renderer/SpotLight.h"
#include "renderer/Texture.h"
#include "scene/Scene.h"

namespace Engine
{

void Sandbox::init(Scene& scene)
{
    SpotLight::SpotLightProperties sp{};
    sp.position = {10, 5.0, 15.0};
    sp.direction = {0, -.5, -1.0};
    scene.addSpotLight(CreateRef<SpotLight>(sp));

    PointLight::PointLightProperties p2{};
    p2.position = {-15, 0, 5};
    scene.addPointLight(CreateRef<PointLight>(p2));

    // Managers / asset context
    MaterialManager materialManager;
    ShaderManager shaderManager;
    TextureManager textureManager;
    AssetContext context(materialManager, shaderManager, textureManager);

    // Ground & tree
    scene.addModel(Model("assets/models/ground/ground.gltf", context, {0.0f, -1.0f, 0.0f}, {10.0f, 1.0f, 10.0f}));
    scene.addModel(Model("assets/models/tree/scene.gltf", context, {10.0f, 0.0f, -10.0f}));

    // Cubes (x = -60 .. 40, step 10)
    for (int x = -60; x <= 40; x += 10)
        scene.addModel(Model("assets/models/cube/cube.gltf", context, {(float)x, 0.0f, 0.0f}));
}

void Sandbox::update(double deltaTime)
{
    static double elapsedTime = 0.0;
    elapsedTime += deltaTime;

    const float radius = 80.0f;
    const float height = 40.0f;
    const float speed = 0.05f; // radians per second

    // Circular horizontal movement + gentle vertical bob
    const float x = radius * std::cos((float)elapsedTime * speed);
    const float z = radius * std::sin((float)elapsedTime * speed);
    const float y = height + std::sin((float)elapsedTime * 0.8f) * 5.0f;

    const glm::vec3 newPosition(x, y, z);
    const glm::vec3 target(0.0f, 0.0f, 0.0f);
    const glm::vec3 direction = glm::normalize(target - newPosition);

    // Example (kept commented like your original):
    // m_movingLight->setPosition(newPosition);
    // m_movingLight->setTarget(target);
    // m_movingLight->setDirection(direction);
}

} // namespace Engine
