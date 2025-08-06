#include "LightingPass.h"
#include "Framebuffer.h"
#include "Profiler.h"
#include "RendererAPI.h"
#include "Scene.h"
#include "Window.h"
#include "pch.h"
#include <fmt/core.h>
#include <glad/glad.h>

namespace Engine
{

LightingPass::LightingPass()
{
    unsigned int NUM_LIGHTS = 256;

    glGenBuffers(1, &m_uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSceneNode::LightUBO) * NUM_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uboLights);

    m_frameBuffer = CreateRef<Framebuffer>();

    m_renderTargetTexture = CreateRef<Texture>(
        TextureProperties{
            .internalFormat = GL_RGBA16F,
            .width = (int)Window::dimensions.x,
            .height = (int)Window::dimensions.y,
            .format = GL_RGBA,
            .type = GL_FLOAT,
        },
        CustomProperties{
            .attachmentType = GL_COLOR_ATTACHMENT0,
        });

    m_frameBuffer->attachTexture(m_renderTargetTexture);

    auto depthTexture = CreateRef<Texture>(
        TextureProperties{
            .internalFormat = GL_DEPTH_COMPONENT24,
            .width = (int)Window::dimensions.x,
            .height = (int)Window::dimensions.y,
            .format = GL_DEPTH_COMPONENT,
            .type = GL_FLOAT,
        },
        CustomProperties{.attachmentType = GL_DEPTH_ATTACHMENT,
                         .minFilter = GL_NEAREST,
                         .magFilter = GL_NEAREST,
                         .wrapS = GL_CLAMP_TO_EDGE,
                         .wrapT = GL_CLAMP_TO_EDGE,
                         .mipmaps = false});

    m_frameBuffer->attachTexture(depthTexture);
}

LightingPass::~LightingPass()
{
    glDeleteBuffers(1, &m_uboLights);
}

void LightingPass::execute(Scene& scene, int& drawCalls)
{
    scene.setActiveCamera(scene.getDefaultCamera());

    m_frameBuffer->bind();

    for (const auto& [material, meshMap] : scene.getRenderQueue("Lighting Pass"))
    {
        material->bind();
        material->update();
        updateUniforms(scene, material);

        if (material->isDoubleSided)
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
            RendererAPI::drawInstanced(mesh, transforms);
            Profiler::registerDrawCall("Lighting Pass");
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        material->unbind();
    }

    m_frameBuffer->unbind();
}

void LightingPass::updateUniforms(Scene& scene, const Ref<Material>& material)
{
    const auto viewMatrix = scene.getActiveCamera()->getViewMatrix();
    const auto projectionMatrix = scene.getActiveCamera()->getProjectionMatrix();
    const auto viewProjectionMatrix = projectionMatrix * viewMatrix;
    const auto camPos = scene.getActiveCamera()->getPosition();

    material->setUniformMatrix4fv("u_ViewProjection", viewProjectionMatrix);
    material->setUniform3fv("viewPos", camPos);

    std::vector<Ref<LightSceneNode>> lights = scene.getLightSceneNodes();

    // Shadows
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
    material->setUniform1i("u_numLights", lights.size());

    GLuint uboBindingPoint = 0;
    // TODO: expensive call, should be cached
    GLuint lightUniformBlockIndex = glGetUniformBlockIndex(material->getShader()->getProgramID(), "LightsBlock");

    if (lightUniformBlockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(material->getShader()->getProgramID(), lightUniformBlockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_uboLights);

        glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_lightBuffer), m_lightBuffer);
    }
}

} // namespace Engine