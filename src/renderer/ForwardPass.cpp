#include "ForwardPass.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "RendererAPI.h"
#include "RendererTypes.h"
#include "Scene.h"
#include "Window.h"
#include <fmt/core.h>
#include <glad/glad.h>

ForwardPass::ForwardPass()
{
    unsigned int NUM_LIGHTS = 256;

    glGenBuffers(1, &m_uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSceneNode::LightUBO) * NUM_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboLights);
}

ForwardPass::~ForwardPass()
{
    glDeleteBuffers(1, &m_uboLights);
}

void ForwardPass::execute()
{
    glColorMask(true, true, true, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::getFrameBufferDimensions().x, Window::getFrameBufferDimensions().y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Scene::setActiveCamera(Scene::getDefaultCamera());

    for (const auto& [material, meshMap] : Scene::getRenderQueue([](const Ref<MeshSceneNode>& node) { return true; }))
    {
        material->bind();
        updateUniforms(material);

        for (const auto& [mesh, transforms] : meshMap)
        {
            RendererAPI::drawInstanced(mesh, transforms);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        material->unbind();
    }
}

void ForwardPass::updateUniforms(const Ref<Material>& material)
{
    const auto viewMatrix = Scene::getActiveCamera()->getViewMatrix();
    const auto projectionMatrix = Scene::getActiveCamera()->getProjectionMatrix();
    const auto viewProjectionMatrix = projectionMatrix * viewMatrix;
    const auto camPos = Scene::getActiveCamera()->getPosition();

    material->setUniformMatrix4fv("u_ViewProjection", viewProjectionMatrix);
    material->setUniform3fv("viewPos", camPos);

    GLuint uboBindingPoint = 0;
    GLuint lightUniformBlockIndex = glGetUniformBlockIndex(material->getShader()->getProgramID(), "LightsBlock");
    if (lightUniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(material->getShader()->getProgramID(), lightUniformBlockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_uboLights);

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_lightBuffer), m_lightBuffer);
    }

    auto lights = Scene::getLightSceneNodes();
    for (size_t i = 0; i < lights.size(); ++i)
    {
        auto light = lights[i];
        m_lightBuffer[i] = ((lights)[i])->toUBO();

        const auto lightSpaceMatrix =
            light->getShadowCam()->getProjectionMatrix() * light->getShadowCam()->getViewMatrix();

        std::string lightSpaceMatrixUniformName = fmt::format("lightSpaceMatrices[{}]", i);
        material->setUniformMatrix4fv(lightSpaceMatrixUniformName.c_str(), lightSpaceMatrix);

        std::string shadowMapUniformName = fmt::format("shadowMaps[{}]", i);
        if (material->hasUniform(shadowMapUniformName.c_str()))
        {
            // Color = 0, shadow[n] = n + 1
            light->getDepthBuffer()->bind(i + 1);                        // Bind texture to the i-th texture unit
            material->setUniform1i(shadowMapUniformName.c_str(), i + 1); // Tell shader which texture unit to use
        }
    }
}
