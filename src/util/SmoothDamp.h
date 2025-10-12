
#pragma once

#include "pch.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <limits>
#include <type_traits>

namespace Engine
{

// MARK: Scalar
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
inline T SmoothDamp(T current, T target, T& currentVelocity, T smoothTime, T deltaTime,
                    T maxSpeed = std::numeric_limits<T>::infinity())
{
    // Small floor to avoid division by zero / instability
    smoothTime = std::max<T>(T(0.0001), smoothTime);

    // Clamp maximum change
    T maxChange = maxSpeed * smoothTime;
    T change = current - target;
    if (std::isfinite(maxChange))
    {
        change = std::clamp(change, -maxChange, maxChange);
    }
    T tempTarget = current - change;

    // Critically-damped spring integration (approx exp(-ωt))
    T omega = T(2) / smoothTime;
    T x = omega * deltaTime;
    T exp = T(1) / (T(1) + x + T(0.48) * x * x + T(0.235) * x * x * x);

    T temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;

    T output = tempTarget + (change + temp) * exp;

    // Prevent overshoot
    // If we moved past the original target, snap to it and zero velocity
    if ((target - current) * (output - target) > T(0))
    {
        output = target;
        currentVelocity = T(0);
    }
    return output;
}

// MARK: glm::vecN
template <typename Vec>
inline Vec SmoothDamp(const Vec& current, const Vec& target, Vec& currentVelocity, float smoothTime, float deltaTime,
                      float maxSpeed = std::numeric_limits<float>::infinity())
{
    static_assert(std::is_same_v<Vec, glm::vec2> || std::is_same_v<Vec, glm::vec3> || std::is_same_v<Vec, glm::vec4>,
                  "SmoothDamp vector overload supports glm::vec2/vec3/vec4");

    smoothTime = std::max(0.0001f, smoothTime);

    // Clamp maximum change magnitude
    float maxChange = maxSpeed * smoothTime;
    Vec change = current - target;
    if (std::isfinite(maxChange))
    {
        float len2 = glm::length2(change);
        float max2 = maxChange * maxChange;
        if (len2 > max2)
        {
            change *= (maxChange / std::sqrt(len2));
        }
    }
    Vec tempTarget = current - change;

    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    Vec temp = (currentVelocity + change * omega) * deltaTime;
    currentVelocity = (currentVelocity - temp * omega) * exp;

    Vec output = tempTarget + (change + temp) * exp;

    // Prevent overshoot (use dot to test passing the target)
    Vec toTarget0 = target - current;
    Vec toTarget1 = output - target;
    if (glm::dot(toTarget0, toTarget1) > 0.0f)
    {
        output = target;
        currentVelocity = Vec(0.0f);
    }
    return output;
}

} // namespace Engine
