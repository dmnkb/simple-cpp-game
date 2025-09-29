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

// MARK: ComputeEffectiveRange()
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

// MARK: LightingPass()
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

// MARK: execute()
void LightingPass::execute(Scene& scene, const LightingInputs& litIn)
{
    m_frameBuffer->bind();

    for (const auto& [material, meshMap] : scene.getRenderQueue("Lighting Pass"))
    {
        material->bind();
        material->update();
        uploadUniforms(scene, material, litIn);

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

// MARK: uploadUniforms()
void LightingPass::uploadUniforms(Scene& scene, const Ref<Material>& material, const LightingInputs& litIn)
{
    // Camera
    const auto cam = scene.getActiveCamera();
    const auto view = cam->getViewMatrix();
    const auto proj = cam->getProjectionMatrix();
    const auto vp = proj * view;
    const auto camPos = cam->getPosition();

    material->setUniformMatrix4fv("u_ViewProjection", vp);
    material->setUniform3fv("viewPos", camPos);

    // MARK: • Spot lights
    const std::vector<Ref<SpotLight>> lights = scene.getSpotLights();
    const int count = std::min<int>((int)lights.size(), MAX_SPOT_LIGHTS);

    // Spot light UBO
    for (int i = 0; i < count; ++i)
    {
        const auto& light = lights[i];
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

    // Shadows array bindings & per-light matrices/layers
    constexpr GLint SHADOW_TU = 5; // Shadow Texture Unit is arbitrary. Needs to be a free one
    if (litIn.shadows.spotShadowArray)
    {
        // Bind shadow array to the chosen unit
        litIn.shadows.spotShadowArray->bind(SHADOW_TU);

        // Ensure linear filtering
        glActiveTexture(GL_TEXTURE0 + SHADOW_TU);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (material->hasUniform("uSpotLightShadowMapArray"))
            material->setUniform1i("uSpotLightShadowMapArray", SHADOW_TU);
    }

    // Upload light-space matrix array in one call
    {
        glm::mat4 lightVP[MAX_SPOT_LIGHTS];
        for (int i = 0; i < count; ++i)
        {
            const auto& light = lights[i];
            lightVP[i] = light->getShadowCam()->getProjectionMatrix() * light->getShadowCam()->getViewMatrix();
        }
        material->getShader()->setUniformMatrix4fvArray("uSpotLightSpaceMatrices[0]", count,
                                                        glm::value_ptr(lightVP[0]));
    }

    if (material->hasUniform("uSpotLightCount"))
        material->setUniform1i("uSpotLightCount", count);
}

} // namespace Engine
