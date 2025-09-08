#include "LightingPass.h"
#include "Framebuffer.h"
#include "RendererAPI.h"
#include "core/Profiler.h"
#include "core/Window.h"
#include "pch.h"
#include "scene/Scene.h"
#include <algorithm>
#include <cmath>
#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{

// Helper: compute effective range from intensity & attenuation
static float ComputeEffectiveRange(float intensity, const glm::vec3& att, float cutoff = 0.01f)
{
    cutoff = glm::max(cutoff, 1e-6f);
    intensity = glm::max(intensity, 1e-6f);

    const float kc = att.x, kl = att.y, kq = att.z;
    const float A = kq, B = kl, C = kc - (intensity / cutoff);

    if (std::abs(A) < 1e-12f)
    {
        if (B > 1e-12f)
            return glm::max(-C / B, 0.0f);
        return (C < 0.0f) ? 1e6f : 0.0f;
    }
    const double disc = double(B) * double(B) - 4.0 * double(A) * double(C);
    if (disc <= 0.0)
        return 0.0f;
    const double d = (-double(B) + std::sqrt(disc)) / (2.0 * double(A));
    return d > 0.0 ? float(d) : 0.0f;
}

LightingPass::LightingPass()
{
    // --- Create the SpotLights UBO (binding = 0, matches GLSL) ---
    glGenBuffers(1, &m_spotLightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLightsUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, /*binding*/ 0, m_spotLightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
    m_frameBuffer->dynamicSize = true;
}

LightingPass::~LightingPass()
{
    glDeleteBuffers(1, &m_spotLightsUBO);
}

void LightingPass::execute(Scene& scene, const LightingInputs& litIn)
{
    m_frameBuffer->bind();

    for (const auto& [material, meshMap] : scene.getRenderQueue("Lighting Pass"))
    {
        material->bind();
        material->update();
        updateUniforms(scene, material, litIn);

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

void LightingPass::updateUniforms(Scene& scene, const Ref<Material>& material, const LightingInputs& litIn)
{
    // Camera
    const auto cam = scene.getActiveCamera();
    const auto view = cam->getViewMatrix();
    const auto proj = cam->getProjectionMatrix();
    const auto vp = proj * view;
    const auto camPos = cam->getPosition();

    material->setUniformMatrix4fv("u_ViewProjection", vp);
    material->setUniform3fv("viewPos", camPos);

    // Collect spot lights from the scene
    const std::vector<Ref<SpotLight>> lights = scene.getSpotLights();

    // Clamp to UBO capacity
    const int count = std::min<int>((int)lights.size(), MAX_SPOT_LIGHTS);

    // -------- Spot light UBO (positions, directions, colors, cones, attenuations) --------
    for (int i = 0; i < count; ++i)
    {
        const auto& light = lights[i];
        const auto props = light->getSpotLightProperties();

        const glm::vec3 pos = props.position;
        const glm::vec3 dirIn =
            (glm::length2(props.direction) < 1e-12f) ? glm::vec3(0, 0, -1) : glm::normalize(props.direction);

        m_spotLightsCPU.positionsWS[i] = glm::vec4(pos, 1.0f);
        // Store INward direction in UBO (toward where the light looks)
        m_spotLightsCPU.directionsWS[i] = glm::vec4(dirIn, 0.0f);
        m_spotLightsCPU.colorsIntensity[i] = props.colorIntensity; // rgb + intensity

        const float innerDeg = glm::clamp(props.coneInner, 0.0f, 89.0f);
        const float outerDeg = glm::clamp(std::max(props.coneOuter, innerDeg), 0.0f, 89.0f);
        const float innerCos = std::cos(glm::radians(innerDeg));
        const float outerCos = std::cos(glm::radians(outerDeg));

        const float range = std::max(ComputeEffectiveRange(props.colorIntensity.w, props.attenuation, 0.01f), 1.0f);

        m_spotLightsCPU.conesRange[i] = glm::vec4(innerCos, outerCos, range, 0.0f);
        m_spotLightsCPU.attenuations[i] = glm::vec4(props.attenuation, 0.0f);

        // Configure shadow camera to look INTO the cone
        auto shadowCam = light->getShadowCam();
        shadowCam->setPosition(pos);

        // *** IMPORTANT: look along +dirIn (into the cone), matching the demo ***
        shadowCam->lookAt(pos + dirIn);

        // Match shadow camera FOV to outer cone
        shadowCam->setPerspective(glm::radians(outerDeg * 2.0f), 1.0f, 0.1f, std::max(range, 1.0f));
    }
    for (int i = count; i < MAX_SPOT_LIGHTS; ++i)
    {
        m_spotLightsCPU.positionsWS[i] = glm::vec4(0.0f);
        m_spotLightsCPU.directionsWS[i] = glm::vec4(0.0f);
        m_spotLightsCPU.colorsIntensity[i] = glm::vec4(0.0f);
        m_spotLightsCPU.conesRange[i] = glm::vec4(0.0f);
        m_spotLightsCPU.attenuations[i] = glm::vec4(0.0f);
    }
    m_spotLightsCPU.meta = glm::vec4((float)count, 0.0f, 0.0f, 0.0f);

    // Upload UBO
    {
        GLuint uboBindingPoint = 0; // must match GLSL binding=0
        GLuint prog = material->getShader()->getProgramID();
        GLuint blockIndex = glGetUniformBlockIndex(prog, "SpotLights");
        if (blockIndex != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(prog, blockIndex, uboBindingPoint);
            glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_spotLightsUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLightsUBO), &m_spotLightsCPU);
        }
    }

    // -------- Shadow array bindings & per-light matrices/layers --------
    constexpr GLint SHADOW_TU = 5; // choose a free unit
    if (litIn.shadows.spotShadowArray)
    {
        // Bind shadow array to the chosen unit
        litIn.shadows.spotShadowArray->bind(SHADOW_TU);

        // Ensure compare + linear filtering are set (parity with demo)
        glActiveTexture(GL_TEXTURE0 + SHADOW_TU);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Demo uses EDGE; BORDER also works. Use EDGE for parity.
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if (material->hasUniform("uShadowMapArray"))
            material->setUniform1i("uShadowMapArray", SHADOW_TU);

        if (material->hasUniform("uShadowLayerCount"))
            material->setUniform1i("uShadowLayerCount", litIn.shadows.layers);

        if (material->hasUniform("uShadowMapResolution"))
            material->setUniform1f("uShadowMapResolution", (float)litIn.shadows.resolution);
    }

    // -------- Upload light-space matrix array in one call (critical) --------
    {
        glm::mat4 lightVP[MAX_SPOT_LIGHTS];
        for (int i = 0; i < count; ++i)
        {
            const auto& light = lights[i];
            lightVP[i] = light->getShadowCam()->getProjectionMatrix() * light->getShadowCam()->getViewMatrix();
        }
        GLuint prog = material->getShader()->getProgramID();
        GLint loc0 = glGetUniformLocation(prog, "lightSpaceMatrices[0]");
        if (loc0 >= 0)
            glUniformMatrix4fv(loc0, count, GL_FALSE, glm::value_ptr(lightVP[0]));
    }

    // -------- Upload layer indices as an array from [0] --------
    {
        GLint layerIdx[MAX_SPOT_LIGHTS] = {};
        for (int i = 0; i < count; ++i)
            layerIdx[i] = i;

        GLuint prog = material->getShader()->getProgramID();
        GLint loc0 = glGetUniformLocation(prog, "uSpotShadowLayer[0]");
        if (loc0 >= 0)
            glUniform1iv(loc0, count, layerIdx);
    }

    if (material->hasUniform("u_numLights"))
        material->setUniform1i("u_numLights", count);
}

} // namespace Engine
