#include "LightingPass.h"
#include "Framebuffer.h"
#include "RendererAPI.h"
#include "core/Profiler.h"
#include "core/Window.h"
#include "glm/gtx/string_cast.hpp"
#include "pch.h"
#include "scene/Scene.h"
#include <algorithm>
#include <cmath>
#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{

LightingPass::LightingPass()
{
    // Spot UBO (binding = 0)
    glGenBuffers(1, &m_spotLightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLightsUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_spotLightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Point UBO (binding = 1)
    glGenBuffers(1, &m_pointLightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightsUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_pointLightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // MARK: Color FB
    m_frameBuffer = CreateRef<Framebuffer>();

    TextureProperties props{};
    props.internalFormat = GL_RGBA16F;
    props.width = (int)Window::frameBufferDimensions.x;
    props.height = (int)Window::frameBufferDimensions.y;
    props.format = GL_RGBA;
    props.type = GL_FLOAT;

    CustomProperties customProps{};
    customProps.attachmentType = GL_COLOR_ATTACHMENT0;
    customProps.path = "Lighting Pass Buffer";

    m_renderTargetTexture = CreateRef<Texture>(props, customProps);

    // MARK: Depth FB
    TextureProperties depthProps{};
    depthProps.internalFormat = GL_DEPTH_COMPONENT24;
    depthProps.width = (int)Window::frameBufferDimensions.x;
    depthProps.height = (int)Window::frameBufferDimensions.y;
    depthProps.format = GL_DEPTH_COMPONENT;
    depthProps.type = GL_FLOAT;

    CustomProperties depthCustomProps{};
    depthCustomProps.attachmentType = GL_DEPTH_ATTACHMENT;
    depthCustomProps.minFilter = GL_NEAREST;
    depthCustomProps.magFilter = GL_NEAREST;
    depthCustomProps.wrapS = GL_CLAMP_TO_EDGE;
    depthCustomProps.wrapT = GL_CLAMP_TO_EDGE;
    depthCustomProps.mipmaps = false;

    Ref<Texture> depthTexture = CreateRef<Texture>(depthProps, depthCustomProps);

    m_frameBuffer->attachTexture(m_renderTargetTexture);
    m_frameBuffer->attachTexture(depthTexture);
    m_frameBuffer->dynamicSize = true;
}

LightingPass::~LightingPass()
{
    glDeleteBuffers(1, &m_spotLightsUBO);
    glDeleteBuffers(1, &m_pointLightsUBO);
}

LightingOutputs LightingPass::execute(Scene& scene, const LightingInputs& lightInputs)
{
    LightingOutputs out{};

    // MARK: Bind FB
    m_frameBuffer->bind();

    for (const auto& [material, meshMap] : scene.getRenderQueue("Lighting Pass"))
    {
        material->bind();
        material->update();
        uploadUniforms(scene, material, lightInputs);

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
    out.renderTargetTexture = m_renderTargetTexture;
    return out;
}

// MARK: Upload uniforms (spot + point + shadows)
void LightingPass::uploadUniforms(Scene& scene, const Ref<Material>& material, const LightingInputs& lightInputs)
{
    // Camera
    const auto cam = scene.getActiveCamera();
    const auto vp = cam->getProjectionMatrix() * cam->getViewMatrix();
    const auto camPos = cam->getPosition();

    material->setUniformMatrix4fv("uViewProjection", vp);
    material->setUniform3fv("uViewPos", camPos);

    // MARK: Spot lights
    const std::vector<Ref<SpotLight>> spotLights = scene.getSpotLights();
    const int spotCount = std::min<int>((int)spotLights.size(), MAX_SPOT_LIGHTS);

    for (int i = 0; i < spotCount; ++i)
    {
        const auto& light = spotLights[i];
        const auto props = light->getSpotLightProperties();
        const float innerCos = std::cos(glm::radians(props.coneInner));
        const float outerCos = std::cos(glm::radians(props.coneOuter));
        const float range = light->getRange();

        m_spotLightsCPU.positionsWS[i] = glm::vec4(props.position, 1.0f);
        m_spotLightsCPU.directionsWS[i] = glm::vec4(props.direction, 0.0f);
        m_spotLightsCPU.colorsIntensity[i] = props.colorIntensity;
        m_spotLightsCPU.conesRange[i] = glm::vec4(innerCos, outerCos, range, 0.0f);
        m_spotLightsCPU.attenuations[i] = glm::vec4(props.attenuation, 0.0f);
    }

    // TODO: Binding point should be configured somewhere, this is too random
    // Bind SpotLights block to binding=0
    {
        GLuint prog = material->getShader()->getProgramID();
        if (GLuint blockIndex = glGetUniformBlockIndex(prog, "SpotLights"); blockIndex != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(prog, blockIndex, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_spotLightsUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLightsUBO), &m_spotLightsCPU);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }

    // Spot shadow array texture + matrices + count
    {
        constexpr GLint SPOT_SHADOW_TU = 5; // Shadow Texture Unit is arbitrary. Needs to be a free one
        if (lightInputs.spotShadowArray)
        {
            lightInputs.spotShadowArray->bind(SPOT_SHADOW_TU);
            // Ensure linear filtering
            // TODO: Check, why this needs to be activated "again" (Should be during creation)
            // glActiveTexture(GL_TEXTURE0 + SPOT_SHADOW_TU);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (material->hasUniform("uSpotLightShadowMapArray"))
                material->setUniform1i("uSpotLightShadowMapArray", SPOT_SHADOW_TU);
        }

        // MARK: SL matrices
        {
            glm::mat4 lightVP[MAX_SPOT_LIGHTS];
            for (int i = 0; i < spotCount; ++i)
            {
                const auto& light = spotLights[i];
                lightVP[i] = light->getShadowCam()->getProjectionMatrix() * light->getShadowCam()->getViewMatrix();
            }
            material->getShader()->setUniformMatrix4fvArray("uSpotLightSpaceMatrices[0]", spotCount,
                                                            glm::value_ptr(lightVP[0]));

            if (material->hasUniform("uSpotLightCount"))
                material->setUniform1i("uSpotLightCount", spotCount);
        }

        // MARK: Point lights
        const std::vector<Ref<PointLight>> pointLights = scene.getPointLights();
        const int pointCount = std::min<int>((int)pointLights.size(), MAX_POINT_LIGHTS);

        for (int i = 0; i < pointCount; ++i)
        {
            const auto& light = pointLights[i];
            const auto props = light->getPointLightProperties();
            const float range = light->getRange();

            m_pointLightsCPU.positionsWS[i] = glm::vec4(props.position, 1.0f);
            m_pointLightsCPU.colorsIntensity[i] = props.colorIntensity;
            m_pointLightsCPU.ranges[i] = glm::vec4(range, 0.0f, 0.0f, 0.0f);
            m_pointLightsCPU.attenuations[i] = glm::vec4(props.attenuation, 0.0f);
        }

        // TODO: Binding point should be configured somewhere, this is too random
        // Bind PointLights block to binding=1
        {
            GLuint prog = material->getShader()->getProgramID();
            if (GLuint blockIndex = glGetUniformBlockIndex(prog, "PointLights"); blockIndex != GL_INVALID_INDEX)
            {
                glUniformBlockBinding(prog, blockIndex, 1);
                glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_pointLightsUBO);
                glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightsUBO);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightsUBO), &m_pointLightsCPU);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
        }

        // Point shadow cubemap array texture + count
        {
            // TODO: Should be configured somewhere, this is too random
            constexpr GLint POINT_SHADOW_TU = 6;
            if (lightInputs.pointShadowCubeArray)
            {
                lightInputs.pointShadowCubeArray->bind(POINT_SHADOW_TU);

                if (material->hasUniform("uPointLightShadowCubeArray"))
                    material->setUniform1i("uPointLightShadowCubeArray", POINT_SHADOW_TU);
            }

            if (material->hasUniform("uPointLightCount"))
                material->setUniform1i("uPointLightCount", pointCount);
        }
    }
}

} // namespace Engine
