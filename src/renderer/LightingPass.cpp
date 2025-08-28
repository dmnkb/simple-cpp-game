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

void LightingPass::execute(Scene& scene)
{
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

    // Fill SoA arrays
    for (int i = 0; i < count; ++i)
    {
        const auto& light = lights[i];

        const SpotLight::SpotLightProperties props = light->getSpotLightProperties();

        const glm::vec3 pos = props.position;
        const glm::vec3 dir =
            (glm::length2(props.direction) < 1e-12f) ? glm::vec3(0, 0, -1) : glm::normalize(props.direction);

        const float innerDeg = glm::clamp(props.coneInner, 0.0f, 89.0f);
        const float outerDeg = glm::clamp(std::max(props.coneOuter, innerDeg), 0.0f, 89.0f);
        const float innerCos = std::cos(glm::radians(innerDeg));
        const float outerCos = std::cos(glm::radians(outerDeg));

        const float range = std::max(ComputeEffectiveRange(props.colorIntensity.w, props.attenuation, 0.01f), 1.0f);

        m_spotLightsCPU.positionsWS[i] = glm::vec4(pos, 1.0f);
        m_spotLightsCPU.directionsWS[i] = glm::vec4(dir, 0.0f);
        m_spotLightsCPU.colorsIntensity[i] = props.colorIntensity; // rgb + intensity
        m_spotLightsCPU.conesRange[i] = glm::vec4(innerCos, outerCos, range, 0.0f);
        m_spotLightsCPU.attenuations[i] = glm::vec4(props.attenuation, 0.0f);

        // Shadow data (matrices & textures) — unchanged
        const glm::mat4 lightSpace =
            light->getShadowCam()->getProjectionMatrix() * light->getShadowCam()->getViewMatrix();

        const std::string lightSpaceName = fmt::format("lightSpaceMatrices[{}]", i);
        material->setUniformMatrix4fv(lightSpaceName.c_str(), lightSpace);

        const std::string shadowMapName = fmt::format("shadowMaps[{}]", i);
        if (material->hasUniform(shadowMapName.c_str()))
        {
            light->getShadowDepthTexture()->bind(i + 1); // color = 0, shadows start at 1
            material->setUniform1i(shadowMapName.c_str(), i + 1);
        }
    }

    // Zero the rest
    for (int i = count; i < MAX_SPOT_LIGHTS; ++i)
    {
        m_spotLightsCPU.positionsWS[i] = glm::vec4(0.0f);
        m_spotLightsCPU.directionsWS[i] = glm::vec4(0.0f);
        m_spotLightsCPU.colorsIntensity[i] = glm::vec4(0.0f);
        m_spotLightsCPU.conesRange[i] = glm::vec4(0.0f);
        m_spotLightsCPU.attenuations[i] = glm::vec4(0.0f);
    }

    // Meta: store count (float to avoid int UBO quirks on older drivers)
    m_spotLightsCPU.meta = glm::vec4((float)count, 0.0f, 0.0f, 0.0f);

    // Upload once per material (bound to same binding point for all materials using the block)
    GLuint uboBindingPoint = 0; // must match GLSL binding
    GLuint blockIndex = glGetUniformBlockIndex(material->getShader()->getProgramID(), "SpotLights");
    if (blockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(material->getShader()->getProgramID(), blockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_spotLightsUBO);

        glBindBuffer(GL_UNIFORM_BUFFER, m_spotLightsUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLightsUBO), &m_spotLightsCPU);
    }

    // Optionally also set a traditional uniform if shader uses it:
    if (material->hasUniform("u_numLights"))
    {
        material->setUniform1i("u_numLights", count);
    }
}

} // namespace Engine
