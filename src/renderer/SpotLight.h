#pragma once
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

class SpotLight
{
  public:
    struct SpotLightProperties
    {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};              // world-space
        glm::vec3 direction = {0.0f, 0.0f, -1.0f};            // world-space, normalized
        glm::vec4 colorIntensity = {1.0f, 1.0f, 1.0f, 10.0f}; // rgb + intensity (scalar)
        float coneInner = 20.0f;                              // degrees
        float coneOuter = 30.0f;                              // degrees
        glm::vec3 attenuation = {1.0f, 0.09f, 0.032f};        // (constant, linear, quadratic)
    };

  public:
    explicit SpotLight(const SpotLightProperties& props);

    SpotLight(); // NEW: default ctor
    void setPosition(const glm::vec3& position = {0.f, 0.f, 0.f})
    {
        m_properties.position = position;
        if (m_shadowCam)
            m_shadowCam->setPosition(position);
    }

    // Point the light at a world-space target
    void setTarget(const glm::vec3& target = {0.f, 0.f, 0.f})
    {
        glm::vec3 dir = target - m_properties.position;
        if (glm::length2(dir) < 1e-12f)
            dir = glm::vec3(0, 0, -1);
        m_properties.direction = glm::normalize(dir);
        if (m_shadowCam)
            m_shadowCam->setDirection(m_properties.direction);
        syncShadowCamera_();
    }

    // Set direction explicitly (normalized internally)
    void setDirection(const glm::vec3& dir)
    {
        glm::vec3 d = (glm::length2(dir) < 1e-12f) ? glm::vec3(0, 0, -1) : glm::normalize(dir);
        m_properties.direction = d;
        if (m_shadowCam)
            m_shadowCam->setDirection(d);
        syncShadowCamera_();
    }

    void setCones(float innerDeg, float outerDeg)
    {
        innerDeg = glm::clamp(innerDeg, 0.0f, 89.0f);
        outerDeg = glm::clamp(outerDeg, innerDeg, 89.0f);
        m_properties.coneInner = innerDeg;
        m_properties.coneOuter = outerDeg;
        syncShadowCamera_();
    }

    void setIntensity(float I)
    {
        m_properties.colorIntensity.w = glm::max(I, 0.0f);
    }

    SpotLightProperties getSpotLightProperties() const
    {
        return m_properties;
    }

    Ref<Camera> getShadowCam() const
    {
        return m_shadowCam;
    }
    Ref<Framebuffer> getShadowFrameBuffer() const
    {
        return m_shadowFramebuffer;
    }
    Ref<Texture> getShadowDepthTexture() const
    {
        return m_shadowDepthTexture;
    }
    Ref<Texture> getShadowDebugColorTexture() const
    {
        return m_shadowDebugColorTexture;
    }

  private:
    // Solve for distance where intensity falls to `cutoff` fraction due to attenuation:
    // I_eff = I / (kc + kl*d + kq*d^2) ==> set I_eff = cutoff, solve quadratic for d.
    static float computeEffectiveRange_(float intensity, const glm::vec3& att, float cutoff)
    {
        cutoff = glm::max(cutoff, 1e-6f);
        intensity = glm::max(intensity, 1e-6f);

        const float kc = att.x;
        const float kl = att.y;
        const float kq = att.z;

        // kq*d^2 + kl*d + (kc - I/cutoff) = 0
        const float A = kq;
        const float B = kl;
        const float C = kc - (intensity / cutoff);

        if (std::abs(A) < 1e-12f)
        {
            // Linear or constant falloff
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

    void sanitizeProps_()
    {
        if (glm::length2(m_properties.direction) < 1e-12f)
            m_properties.direction = glm::vec3(0, 0, -1);
        else
            m_properties.direction = glm::normalize(m_properties.direction);

        m_properties.coneInner = glm::clamp(m_properties.coneInner, 0.0f, 89.0f);
        m_properties.coneOuter = glm::clamp(glm::max(m_properties.coneOuter, m_properties.coneInner), 0.0f, 89.0f);
    }

    void syncShadowCamera_()
    {
        if (!m_shadowCam)
            return;
        // Spotlight perspective: FOV ≈ outerCone*2, aspect = 1 for square shadow maps
        const float fovY = glm::radians(glm::clamp(m_properties.coneOuter * 2.0f, 1.0f, 178.0f));
        const float nearP = 0.05f;
        const float farP =
            glm::max(computeEffectiveRange_(m_properties.colorIntensity.w, m_properties.attenuation, 0.01f), 1.0f);
        m_shadowCam->setPerspective(fovY, /*aspect*/ 1.0f, nearP, farP);
        m_shadowCam->setPosition(m_properties.position);
        m_shadowCam->setDirection(m_properties.direction);
    }

  private:
    SpotLightProperties m_properties;

    Ref<Camera> m_shadowCam;
    Ref<Texture> m_shadowDepthTexture;
    Ref<Texture> m_shadowDebugColorTexture;
    Ref<Framebuffer> m_shadowFramebuffer;
};

} // namespace Engine
