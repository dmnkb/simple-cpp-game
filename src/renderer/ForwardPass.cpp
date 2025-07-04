#include "ForwardPass.h"
#include "Framebuffer.h"
#include "RendererAPI.h"
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

void ForwardPass::execute(Scene& scene)
{
    glColorMask(true, true, true, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::frameBufferDimensions.x, Window::frameBufferDimensions.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.setActiveCamera(scene.getDefaultCamera());

    int i = 0;

    for (const auto& [material, meshMap] : scene.getRenderQueue())
    {
        material->bind();
        material->update();
        updateUniforms(scene, material);

        if (material->name == "foliage")
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }

        for (const auto& [mesh, transforms] : meshMap)
        {
            i++;
            RendererAPI::drawInstanced(mesh, transforms);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        material->unbind();
    }

    // std::cout << "number draw calls: " << i << std::endl;
}

void ForwardPass::updateUniforms(Scene& scene, const Ref<Material>& material)
{
    const auto viewMatrix = scene.getActiveCamera()->getViewMatrix();
    const auto projectionMatrix = scene.getActiveCamera()->getProjectionMatrix();
    const auto viewProjectionMatrix = projectionMatrix * viewMatrix;
    const auto camPos = scene.getActiveCamera()->getPosition();

    material->setUniformMatrix4fv("u_ViewProjection", viewProjectionMatrix);
    material->setUniform3fv("viewPos", camPos);

    // Shadows
    auto lights = scene.getLightSceneNodes();
    for (size_t i = 0; i < lights.size(); ++i)
    {
        const auto light = lights[i];
        m_lightBuffer[i] = ((lights)[i])->toUBO();

        const auto lightSpaceMatrix =
            light->getShadowCam()->getProjectionMatrix() * light->getShadowCam()->getViewMatrix();

        std::string lightSpaceMatrixUniformName = fmt::format("lightSpaceMatrices[{}]", i);
        material->setUniformMatrix4fv(lightSpaceMatrixUniformName.c_str(), lightSpaceMatrix);

        std::string shadowMapUniformName = fmt::format("shadowMaps[{}]", i);
        if (material->hasUniform(shadowMapUniformName.c_str()))
        {
            // Color = 0, shadow[n] = n + 1
            light->getShadowDepthTexture()->bind(i + 1);                 // Bind texture to the i-th texture unit
            material->setUniform1i(shadowMapUniformName.c_str(), i + 1); // Tell shader which texture unit to use
        }
    }

    // Lights
    GLuint uboBindingPoint = 0;
    GLuint lightUniformBlockIndex = glGetUniformBlockIndex(material->getShader()->getProgramID(), "LightsBlock");
    if (lightUniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(material->getShader()->getProgramID(), lightUniformBlockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_uboLights);

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_lightBuffer), m_lightBuffer);
    }
}
