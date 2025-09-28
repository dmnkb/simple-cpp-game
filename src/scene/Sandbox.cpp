#include "Sandbox.h"
#include "glm/gtx/string_cast.hpp"
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
    // ---- Spotlights (x = -55 .. 45, step 10) ----
    {
        const float y = 5.0f, z = 5.0f;
        const glm::vec3 dir(-1.0f, -1.0f, -1.0f);
        for (int x = -55; x <= 45; x += 10)
        {
            SpotLight::SpotLightProperties p{};
            p.position = {(float)x, y, z};
            const float rx = (((double)rand() / (RAND_MAX)) - .5f);
            const float ry = (((double)rand() / (RAND_MAX)) - .5f);
            const float rz = (((double)rand() / (RAND_MAX)) - .5f);
            const glm::vec3 randRir(rx, ry, rz);
            std::println("{}", glm::to_string(dir + randRir));
            p.direction = dir + randRir;
            scene.addSpotLight(CreateRef<SpotLight>(p));
        }
    }

    // ---- Managers / asset context ----
    MaterialManager materialManager;
    ShaderManager shaderManager;
    TextureManager textureManager;
    AssetContext context(materialManager, shaderManager, textureManager);

    // ---- Ground & tree ----
    scene.addModel(Model("assets/models/ground/ground.gltf", context, {0.0f, -1.0f, 0.0f}, {10.0f, 1.0f, 10.0f}));
    scene.addModel(Model("assets/models/tree/scene.gltf", context, {10.0f, 0.0f, -10.0f}));

    // ---- Cubes (x = -60 .. 40, step 10) ----
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
