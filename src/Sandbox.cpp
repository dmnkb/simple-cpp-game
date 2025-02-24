#include "Sandbox.h"
#include "LightSceneNode.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshSceneNode.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "pch.h"

void Sandbox::init()
{
    auto mesh = CreateRef<Mesh>("my/unused/model/path/model.glb");
    auto lightTexture = CreateRef<Texture>("assets/texture_08.png");
    auto shader = CreateRef<Shader>("assets/phong.vs", "assets/phong.fs");

    auto groundMaterial = CreateRef<Material>(shader);
    groundMaterial->setDiffuseMap(lightTexture);
    groundMaterial->setTextureRepeat(256);

    auto blockMaterial = CreateRef<Material>(shader);
    blockMaterial->setDiffuseMap(lightTexture);

    auto groundNode = CreateRef<MeshSceneNode>(mesh, groundMaterial);
    groundNode->setPosition(glm::vec3(0, -1, 0));
    groundNode->setScale(glm::vec3(512, 1, 512));
    Scene::addMeshSceneNode(groundNode);

    auto foo = CreateRef<MeshSceneNode>(mesh, blockMaterial);
    foo->setScale(glm::vec3(2, 2, 2));
    foo->setPosition(glm::vec3(0, .5, 0));
    Scene::addMeshSceneNode(foo);

    // Create floating cubes

    const int numCubes = 16;
    const float radius = 10.0f;

    for (int i = 0; i < numCubes; ++i)
    {
        float angle = glm::radians(i * (360.0f / numCubes));
        float x = radius * glm::cos(angle);
        float z = radius * glm::sin(angle);

        auto floatingNode = CreateRef<MeshSceneNode>(mesh, blockMaterial);
        floatingNode->setName("Floating Cube " + std::to_string(i));
        floatingNode->setPosition(glm::vec3(x, 4.0f, z));
        Scene::addMeshSceneNode(floatingNode);
    }

    auto light1 = CreateRef<LightSceneNode>(glm::vec3(10, 15, -10), glm::vec3(1, 0, 0), glm::vec3(-1, -1, 0), ELT_SPOT);
    Scene::addLightSceneNode(light1);

    auto light2 = CreateRef<LightSceneNode>(glm::vec3(10, 15, 0), glm::vec3(0, 1, 0), glm::vec3(-1, -1, 0), ELT_SPOT);
    Scene::addLightSceneNode(light2);

    auto light3 = CreateRef<LightSceneNode>(glm::vec3(10, 15, 10), glm::vec3(0, 0, 1), glm::vec3(-1, -1, 0), ELT_SPOT);
    Scene::addLightSceneNode(light3);

    auto light4 = CreateRef<LightSceneNode>(glm::vec3(100.0f, 200.0f, 100.0f), // Position of the sun
                                            glm::vec3(0.8f, 0.7f, 0.6f),       // Softer warm color
                                            glm::vec3(-0.5f, -1.0f, -0.5f),    // Direction of sunlight
                                            ELT_DIRECTIONAL                    // Type: Directional light
    );

    Scene::addLightSceneNode(light4);
}

static float s_increment = 0.0f;

void Sandbox::update(double deltaTime)
{
    s_increment += 0.5 * deltaTime;

    const int numCubes = 16;
    const float radius = 10.0f;

    for (int i = 0; i < numCubes; ++i)
    {
        std::string nodeName = "Floating Cube " + std::to_string(i);
        const auto testNode = Scene::getByName(nodeName);
        if (testNode && Is(Ref<MeshSceneNode>, *testNode))
        {
            const auto node = Get(Ref<MeshSceneNode>, *testNode);

            float angle = glm::radians(i * (360.0f / numCubes));
            float x = radius * glm::cos(angle);
            float y = glm::sin(s_increment + i) * 2.0f + 4.0f;
            float z = radius * glm::sin(angle + s_increment); // Rotates along the Z-axis

            node->setPosition(glm::vec3(x, y, z));
        }
    }
}
