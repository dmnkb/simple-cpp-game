#include "Sandbox.h"
#include "LearnOpenGLShader.h"
#include "LightSceneNode.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshSceneNode.h"
#include "Model.h"
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

    auto groundNode = CreateRef<MeshSceneNode>(mesh, groundMaterial);
    groundNode->setPosition(glm::vec3(0, -10, 0));
    groundNode->setScale(glm::vec3(512, 1, 512));
    Scene::addMeshSceneNode(groundNode);

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
}

void Sandbox::update(double deltaTime)
{
    // don't forget to enable shader before setting uniforms
    m_OurShader->use();

    const auto viewMatrix = Scene::getActiveCamera()->getViewMatrix();
    const auto projectionMatrix = Scene::getActiveCamera()->getProjectionMatrix();
    const auto viewProjectionMatrix = projectionMatrix * viewMatrix;

    // view/projection transformations
    glm::mat4 projection = Scene::getActiveCamera()->getProjectionMatrix();
    glm::mat4 view = Scene::getActiveCamera()->getViewMatrix();
    m_OurShader->setMat4("projection", projection);
    m_OurShader->setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));     // it's a bit too big for our scene, so scale it down
    m_OurShader->setMat4("model", model);
    m_Model->Draw(m_OurShader);
}
