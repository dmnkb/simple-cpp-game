#include "Sandbox.h"
#include "LearnOpenGLShader.h"
#include "LightSceneNode.h"
#include "Material.h"
#include "Model.h"
#include "Scene.h"
#include "Shader.h"
#include "Texture.h"
#include "pch.h"

void Sandbox::init()
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

    auto light4 = CreateRef<LightSceneNode>(glm::vec3(100.0f, 200.0f, 100.0f), // Position of the sun
                                            glm::vec3(0.8f, 0.7f, 0.6f),       // Softer warm color
                                            glm::vec3(-0.5f, -1.0f, -0.5f),    // Direction of sunlight
                                            ELT_DIRECTIONAL                    // Type: Directional light
    );

    Scene::addLightSceneNode(light4);

    // LearnOpenGL example
    m_OurShader = CreateRef<LearnOpenGLShader>("assets/model_loading.vs", "assets/model_loading.fs");

    // load models
    // -----------
    m_Model = CreateRef<Model>("assets/KhronosGroup glTF-Sample-Assets main Models-Sponza/glTF/Sponza.gltf");
    Scene::addModel(m_Model);
}

void Sandbox::update(double deltaTime) {}
