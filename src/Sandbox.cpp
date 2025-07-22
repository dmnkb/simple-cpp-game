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
    m_movingLight =
        CreateRef<LightSceneNode>(glm::vec3(-10, 25, 50), glm::vec3(1, 1, 1), glm::vec3(0, -1, -1), ELT_SPOT);
    scene.addLightSceneNode(m_movingLight);

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

    scene.addModel(Model("assets/models/tree/scene.gltf", context, {10.0, 0.0, -10.0}));
    scene.addModel(Model("assets/models/tree/scene.gltf", context, {-10.0, 0.0, 10.0}));
    scene.addModel(Model("assets/models/tree/scene.gltf", context, {-10.0, 0.0, -10.0}));
    scene.addModel(Model("assets/models/tree/scene.gltf", context, {10.0, 0.0, 10.0}));

    scene.addModel(Model("assets/models/cube/cube.gltf", context, {0.0, 0.0, 0.0}));
    scene.addModel(Model("assets/models/ground/ground.gltf", context, {0.0, -1, 0.0}, {4.0, 1.0, 4.0}));
}

void Sandbox::update(double deltaTime)
{
    static double elapsedTime = 0.0;
    elapsedTime += deltaTime;

    float radius = 30.0f;
    float height = 25.0f;
    float speed = 0.05f; // radians per second

    // Circular horizontal movement
    float x = radius * cos(elapsedTime * speed);
    float z = radius * sin(elapsedTime * speed);
    float y = height + sin(elapsedTime * 0.8f) * 5.0f; // gentle vertical bobbing

    glm::vec3 newPosition = glm::vec3(x, y, z);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // point light looks at
    glm::vec3 direction = glm::normalize(target - newPosition);

    m_movingLight->setPosition(newPosition);
    m_movingLight->setLookAt({0, 0, 0});
}
