
#pragma once

#include "pch.h"
#include <glm/glm.hpp>

namespace Engine
{

// Solve for distance where intensity falls to `cutoff` fraction due to attenuation:
// I_eff = I / (kc + kl*d + kq*d^2) ==> set I_eff = cutoff, solve quadratic for d.
inline float ComputeEffectiveRange(float intensity, const glm::vec3& att, float cutoff)
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

} // namespace Engine
