#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "util/ComputeEffectiveRange.h"
#include "util/uuid.h"
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

namespace Engine
{

class PointLight
{
  public:
    // TODO: Make this public accessible and add a direty flag, which, if true, the ECS LightUpdateSystem
    // will pick up and use to recalculate the range and cosines (spot)
    struct PointLightProperties
    {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};             // world-space
        glm::vec4 colorIntensity = {1.0f, 1.0f, 1.0f, 1.0f}; // rgb + intensity (scalar in .w)
        glm::vec3 attenuation = {1.0f, 0.09f, 0.032f};       // (constant, linear, quadratic)
    };

  public:
    explicit PointLight(const PointLightProperties& props) : m_properties(props)
    {
        m_properties.colorIntensity.w = glm::max(m_properties.colorIntensity.w, 0.0f);
        syncShadowCameras_();
    }

    PointLight() : PointLight(PointLightProperties{}) {}

    void setPosition(const glm::vec3& position = {0.f, 0.f, 0.f})
    {
        m_properties.position = position;
        syncShadowCameras_();
    }

    void setIntensity(float I)
    {
        m_properties.colorIntensity.w = glm::max(I, 0.0f);
        syncShadowCameras_();
    }

    void setAttenuation(const glm::vec3& att)
    {
        m_properties.attenuation = att;
        syncShadowCameras_();
    }

    PointLightProperties getPointLightProperties() const
    {
        return m_properties;
    }

    float getRange() const
    {
        return m_range;
    }

    // 6 cameras for a cubemap: +X, -X, +Y, -Y, +Z, -Z
    const std::array<Ref<Camera>, 6>& getShadowCams() const
    {
        return m_shadowCams;
    }

    UUID getIdentifier() const
    {
        return identifier;
    }

  private:
    void syncShadowCameras_()
    {
        // Compute a reasonable far plane from attenuation (e.g., 1% cutoff)
        m_range = glm::max(ComputeEffectiveRange(m_properties.colorIntensity.w, m_properties.attenuation, 0.01f), 1.0f);

        // Prepare 6 cameras if not present; set 90° FOV, aspect 1, shared near/far
        const float fovY = glm::radians(90.0f);
        const float nearP = 0.05f;
        const float farP = m_range;

        static const glm::vec3 directions[6] = {
            {1, 0, 0},  // +X
            {-1, 0, 0}, // -X
            {0, 1, 0},  // +Y
            {0, -1, 0}, // -Y
            {0, 0, 1},  // +Z
            {0, 0, -1}  // -Z
        };

        // OpenGL cubemap convention ups for each face
        static const glm::vec3 ups[6] = {
            {0, -1, 0}, // +X
            {0, -1, 0}, // -X
            {0, 0, 1},  // +Y
            {0, 0, -1}, // -Y
            {0, -1, 0}, // +Z
            {0, -1, 0}  // -Z
        };

        for (size_t i = 0; i < 6; ++i)
        {
            if (!m_shadowCams[i])
                m_shadowCams[i] = CreateRef<Camera>();

            m_shadowCams[i]->setPerspective(fovY, /*aspect*/ 1.0f, nearP, farP);
            m_shadowCams[i]->setPosition(m_properties.position);
            m_shadowCams[i]->setDirection(directions[i], ups[i]);
        }
    }

  private:
    UUID identifier;
    PointLightProperties m_properties;
    std::array<Ref<Camera>, 6> m_shadowCams;
    float m_range = 0.0f;
};

} // namespace Engine
