#pragma once
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "Camera.h"
#include "Framebuffer.h"
#include "Texture.h"

namespace Engine
{

class PointLight
{
  public:
    struct PointLightProperties
    {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};              // world-space
        glm::vec4 colorIntensity = {1.0f, 1.0f, 1.0f, 10.0f}; // rgb + intensity (scalar in .w)
        glm::vec3 attenuation = {1.0f, 0.09f, 0.032f};        // (constant, linear, quadratic)
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

  private:
    // Solve for distance where intensity falls to `cutoff` fraction due to attenuation:
    // I_eff = I / (kc + kl*d + kq*d^2) ==> set I_eff = cutoff, solve quadratic for d.
    static float computeEffectiveRange_(float intensity, const glm::vec3& att, float cutoff)
    {
        float c = glm::max(cutoff, 1e-6f);
        float I = glm::max(intensity, 1e-6f);

        const float kc = att.x;
        const float kl = att.y;
        const float kq = att.z;

        // kq*d^2 + kl*d + (kc - I/c) = 0
        const float A = kq;
        const float B = kl;
        const float C = kc - (I / c);

        if (std::abs(A) < 1e-12f)
        {
            if (B > 1e-12f)
                return glm::max(-C / B, 0.0f);
            return (C < 0.0f) ? 1e6f : 0.0f; // infinite-ish or zero
        }

        const double disc = double(B) * double(B) - 4.0 * double(A) * double(C);
        if (disc <= 0.0)
            return 0.0f;

        const double d = (-double(B) + std::sqrt(disc)) / (2.0 * double(A));
        return d > 0.0 ? float(d) : 0.0f;
    }

    void syncShadowCameras_()
    {
        // Compute a reasonable far plane from attenuation (e.g., 1% cutoff)
        m_range =
            glm::max(computeEffectiveRange_(m_properties.colorIntensity.w, m_properties.attenuation, 0.01f), 1.0f);

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
    PointLightProperties m_properties;
    std::array<Ref<Camera>, 6> m_shadowCams;
    float m_range = 0.0f;
};

} // namespace Engine
