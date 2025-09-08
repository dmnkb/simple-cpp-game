#include "SpotLight.h"
#include "Texture.h"
#include "pch.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine
{

SpotLight::SpotLight() : SpotLight(SpotLightProperties{}) // delegate to the main ctor with defaults
{
}

SpotLight::SpotLight(const SpotLightProperties& props) : m_properties(props)
{
    // Sanitize inputs
    if (glm::length2(m_properties.direction) < 1e-12f)
        m_properties.direction = glm::vec3(0, 0, -1);
    else
        m_properties.direction = glm::normalize(m_properties.direction);

    m_properties.coneInner = glm::clamp(m_properties.coneInner, 0.0f, 89.0f);
    m_properties.coneOuter = glm::clamp(glm::max(m_properties.coneOuter, m_properties.coneInner), 0.0f, 89.0f);

    // Compute an effective range from attenuation + intensity (cutoff is the fraction of peak)
    auto computeEffectiveRange = [](float intensity, const glm::vec3& att, float cutoff) -> float
    {
        cutoff = glm::max(cutoff, 1e-6f);
        intensity = glm::max(intensity, 1e-6f);

        const float kc = att.x;
        const float kl = att.y;
        const float kq = att.z;

        // Solve kq*d^2 + kl*d + (kc - I/cutoff) = 0 for positive root
        const float A = kq;
        const float B = kl;
        const float C = kc - (intensity / cutoff);

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
    };

    const float intensity = m_properties.colorIntensity.w;
    float range = computeEffectiveRange(intensity, m_properties.attenuation, 0.01f);
    range = glm::max(range, 1.0f);

    // Shadow camera
    m_shadowCam = CreateRef<Camera>();
    const float fovY = glm::radians(m_properties.coneOuter * 2.0f); // spotlight cone ≈ outer*2
    const float aspect = 1.0f;                                      // square shadow maps
    const float nearP = 0.05f;
    const float farP = glm::max(range, nearP + 0.01f);

    m_shadowCam->setPerspective(fovY, aspect, nearP, farP);
    m_shadowCam->setPosition(m_properties.position);

    // Robust lookAt: pick a stable up vector even when aiming near ±Y
    glm::vec3 forward = glm::normalize(m_properties.direction);
    glm::vec3 up = (std::abs(forward.y) > 0.99f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
    m_shadowCam->lookAt(m_properties.position + forward, up);
}

} // namespace Engine
