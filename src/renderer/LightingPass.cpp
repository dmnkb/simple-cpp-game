#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/Profiler.h"
#include "core/Window.h"
#include "pch.h"
#include "renderer/DirectionalLight.h"
#include "renderer/Framebuffer.h"
#include "renderer/LightingPass.h"
#include "renderer/RendererAPI.h"
#include "renderer/GLDebug.h"
#include "scene/Scene.h"

namespace Engine
{

LightingPass::LightingPass()
{
    // MARK: Shadow UBO binding
    // Spot UBO (binding = 0)
    GLCall(glGenBuffers(1, &m_spotLightsUBO));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO));
    GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLightsUBO), nullptr, GL_DYNAMIC_DRAW));
    GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_spotLightsUBO));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    // Point UBO (binding = 1)
    GLCall(glGenBuffers(1, &m_pointLightsUBO));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightsUBO));
    GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightsUBO), nullptr, GL_DYNAMIC_DRAW));
    GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_pointLightsUBO));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    // Directional UBO (binding = 2)
    GLCall(glGenBuffers(1, &m_directionalLightUBO));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_directionalLightUBO));
    GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightUBO), nullptr, GL_DYNAMIC_DRAW));
    GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_directionalLightUBO));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    // MARK: Framebuffers
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

    // A depth FB is needed for alpha sorting
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

    // MARK: Create Samplers
    // Regular sampler (no comparison)
    GLCall(glGenSamplers(1, &m_samplerRegular));
    GLCall(glSamplerParameteri(m_samplerRegular, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glSamplerParameteri(m_samplerRegular, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glSamplerParameteri(m_samplerRegular, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glSamplerParameteri(m_samplerRegular, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GLCall(glSamplerParameteri(m_samplerRegular, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));
    GLCall(glSamplerParameteri(m_samplerRegular, GL_TEXTURE_COMPARE_MODE, GL_NONE));
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLCall(glSamplerParameterfv(m_samplerRegular, GL_TEXTURE_BORDER_COLOR, borderColor));

    // Comparison sampler (hardware PCF)
    GLCall(glGenSamplers(1, &m_samplerCompare));
    GLCall(glSamplerParameteri(m_samplerCompare, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glSamplerParameteri(m_samplerCompare, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glSamplerParameteri(m_samplerCompare, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glSamplerParameteri(m_samplerCompare, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GLCall(glSamplerParameteri(m_samplerCompare, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));
    GLCall(glSamplerParameteri(m_samplerCompare, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GLCall(glSamplerParameteri(m_samplerCompare, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    GLCall(glSamplerParameterfv(m_samplerCompare, GL_TEXTURE_BORDER_COLOR, borderColor));
}

LightingPass::~LightingPass()
{
    GLCall(glDeleteBuffers(1, &m_spotLightsUBO));
    GLCall(glDeleteBuffers(1, &m_pointLightsUBO));
    GLCall(glDeleteBuffers(1, &m_directionalLightUBO));
    GLCall(glDeleteSamplers(1, &m_samplerRegular));
    GLCall(glDeleteSamplers(1, &m_samplerCompare));
}

// MARK: Execute
LightingOutputs LightingPass::execute(const Ref<Scene>& scene, const LightingInputs& lightInputs)
{
    LightingOutputs out{};

    m_frameBuffer->bind();

    for (const auto& [material, meshMap] : scene->getRenderQueue("Lighting Pass"))
    {
        material->bind();
        material->update();
        uploadUniforms(scene, material, lightInputs);

        if (material->isDoubleSided)
        {
            GLCall(glDisable(GL_CULL_FACE));
        }
        else
        {
            GLCall(glEnable(GL_CULL_FACE));
            GLCall(glCullFace(GL_BACK));
        }

        for (const auto& [mesh, transforms] : meshMap)
        {
            RendererAPI::drawInstanced(mesh, transforms);
            Profiler::registerDrawCall("Lighting Pass");
        }

        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
        material->unbind();
    }

    m_frameBuffer->unbind();
    out.renderTargetTexture = m_renderTargetTexture;
    return out;
}

// MARK: Upload uniforms
void LightingPass::uploadUniforms(const Ref<Scene>& scene, const Ref<Material>& material,
                                  const LightingInputs& lightInputs)
{
    // Camera
    const auto cam = scene->getActiveCamera();
    const auto vp = cam.getProjectionMatrix() * cam.getViewMatrix();
    const auto camPos = cam.getPosition();

    material->setUniformMatrix4fv("uViewProjection", vp);
    material->setUniform3fv("uViewPos", camPos);

    // Ambient light
    material->setUniform4fv("uAmbientLightColor", scene->getAmbientLightColor());

    // MARK: Spot lights
    {
        const std::vector<Ref<SpotLight>> spotLights = scene->getSpotLights();
        const int spotCount = std::min<int>((int)spotLights.size(), MAX_SPOT_LIGHTS);

        // Sync
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

        // UBO (binding = 0) TODO: Binding point should be configured somewhere, this is too random
        {
            GLuint prog = material->getShader()->getProgramID();
            GLuint blockIndex;
            GLCall(blockIndex = glGetUniformBlockIndex(prog, "SpotLights"));
            if (blockIndex != GL_INVALID_INDEX)
            {
                GLCall(glUniformBlockBinding(prog, blockIndex, 0));
                GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_spotLightsUBO));
                GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO));
                GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLightsUBO), &m_spotLightsCPU));
                GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
            }
        }

        // Shadow array
        constexpr GLint SPOT_SHADOW_TU = 5; // Texture Unit is arbitrary. Needs to be a free one
        constexpr GLint SPOT_SHADOW_CMP_TU = 8; // Comparison sampler for hardware PCF
        if (lightInputs.spotShadowArray)
        {
            // Regular sampler (for blocker search - needs depth values)
            lightInputs.spotShadowArray->bind(SPOT_SHADOW_TU);
            GLCall(glBindSampler(SPOT_SHADOW_TU, m_samplerRegular));
            if (material->hasUniform("uSpotLightShadowMapArray"))
                material->setUniform1i("uSpotLightShadowMapArray", SPOT_SHADOW_TU);
            
            // Comparison sampler (for PCF filtering - hardware accelerated)
            lightInputs.spotShadowArray->bind(SPOT_SHADOW_CMP_TU);
            GLCall(glBindSampler(SPOT_SHADOW_CMP_TU, m_samplerCompare));
            if (material->hasUniform("uSpotLightShadowMapArrayCmp"))
                material->setUniform1i("uSpotLightShadowMapArrayCmp", SPOT_SHADOW_CMP_TU);
        }

        // Matrices
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
    }

    // MARK: Point lights
    {
        const std::vector<Ref<PointLight>> pointLights = scene->getPointLights();
        const int pointCount = std::min<int>((int)pointLights.size(), MAX_POINT_LIGHTS);

        // Sync
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

        // UBO (binding = 0) TODO: Binding point should be configured somewhere, this is too random
        {
            GLuint prog = material->getShader()->getProgramID();
            GLuint blockIndex;
            GLCall(blockIndex = glGetUniformBlockIndex(prog, "PointLights"));
            if (blockIndex != GL_INVALID_INDEX)
            {
                GLCall(glUniformBlockBinding(prog, blockIndex, 1));
                GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_pointLightsUBO));
                GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_pointLightsUBO));
                GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightsUBO), &m_pointLightsCPU));
                GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
            }
        }

        // Shadow array
        {
            constexpr GLint POINT_SHADOW_TU = 6; // Regular sampler
            constexpr GLint POINT_SHADOW_CMP_TU = 9; // Comparison sampler for hardware PCF
            if (lightInputs.pointShadowCubeArray)
            {
                // Regular sampler (for blocker search - needs depth values)
                lightInputs.pointShadowCubeArray->bind(POINT_SHADOW_TU);
                GLCall(glBindSampler(POINT_SHADOW_TU, m_samplerRegular));
                if (material->hasUniform("uPointLightShadowCubeArray"))
                    material->setUniform1i("uPointLightShadowCubeArray", POINT_SHADOW_TU);
                
                // Comparison sampler (for PCF filtering - hardware accelerated)
                lightInputs.pointShadowCubeArray->bind(POINT_SHADOW_CMP_TU);
                GLCall(glBindSampler(POINT_SHADOW_CMP_TU, m_samplerCompare));
                if (material->hasUniform("uPointLightShadowCubeArrayCmp"))
                    material->setUniform1i("uPointLightShadowCubeArrayCmp", POINT_SHADOW_CMP_TU);
            }

            if (material->hasUniform("uPointLightCount"))
                material->setUniform1i("uPointLightCount", pointCount);
        }
    }

    // MARK: Directional light
    {
        const Ref<DirectionalLight> directionalLight = scene->getDirectionalLight();

        // Sync
        const DirectionalLight::DirectionalLightProperties props = directionalLight->props();
        m_directionalLightCPU.directionWS = glm::vec4(props.direction, 0.0f);
        m_directionalLightCPU.colorsIntensity = props.colorIntensity;

        // UBO (binding = 2) TODO: Binding point should be configured somewhere, this is too random
        {
            GLuint prog = material->getShader()->getProgramID();
            GLuint blockIndex;
            GLCall(blockIndex = glGetUniformBlockIndex(prog, "DirectionalLight"));
            if (blockIndex != GL_INVALID_INDEX)
            {
                GLCall(glUniformBlockBinding(prog, blockIndex, 2));
                GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_directionalLightUBO));
                GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_directionalLightUBO));
                GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirectionalLightUBO), &m_directionalLightCPU));
                GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
            }
        }

        // Shadow array
        constexpr GLint DIRECTIONAL_SHADOW_TU = 7; // Regular sampler
        constexpr GLint DIRECTIONAL_SHADOW_CMP_TU = 10; // Comparison sampler for hardware PCF
        if (lightInputs.directionalShadowArray)
        {
            // Regular sampler (for blocker search - needs depth values)
            lightInputs.directionalShadowArray->bind(DIRECTIONAL_SHADOW_TU);
            GLCall(glBindSampler(DIRECTIONAL_SHADOW_TU, m_samplerRegular));
            if (material->hasUniform("uDirectionalLightShadowMapArray"))
                material->setUniform1i("uDirectionalLightShadowMapArray", DIRECTIONAL_SHADOW_TU);
            
            // Comparison sampler (for PCF filtering - hardware accelerated)
            lightInputs.directionalShadowArray->bind(DIRECTIONAL_SHADOW_CMP_TU);
            GLCall(glBindSampler(DIRECTIONAL_SHADOW_CMP_TU, m_samplerCompare));
            if (material->hasUniform("uDirectionalLightShadowMapArrayCmp"))
                material->setUniform1i("uDirectionalLightShadowMapArrayCmp", DIRECTIONAL_SHADOW_CMP_TU);
        }

        // Matrices
        {
            glm::mat4 lightVP[kCascadeCount];
            for (int cascade = 0; cascade < kCascadeCount; ++cascade)
            {
                const auto cam = directionalLight->getShadowCams()[cascade];
                if (!cam.has_value())
                    return;
                lightVP[cascade] = cam->getProjectionMatrix() * cam->getViewMatrix();
            }
            material->getShader()->setUniformMatrix4fvArray("uDirectionalLightSpaceMatrix[0]", kCascadeCount,
                                                            glm::value_ptr(lightVP[0]));
        }
    }
}

} // namespace Engine
