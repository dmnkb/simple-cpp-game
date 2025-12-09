#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "core/Event.h"
#include "core/EventManager.h"
#include "renderer/Camera.h"
#include "util/uuid.h"

namespace Engine
{

// TODO: Store centralized
inline constexpr std::size_t kCascadeCount = 4;

class DirectionalLight
{
  public:
    struct DirectionalLightProperties
    {
        glm::vec3 direction = glm::normalize(glm::vec3(-1.f, -1.f, -1.f)); // from light → scene
        glm::vec4 colorIntensity = {1, 1, 1, 1};
    };

    // main camera snapshot to build cascades
    struct MainCamData
    {
        glm::vec3 pos{0};
        glm::vec3 dir{0, 0, -1};
        glm::vec3 up{0, 1, 0};
        float fovY = glm::radians(60.f);
        float aspect = 16.f / 9.f;
        float zNear = 0.1f, zFar = 200.f;
        bool valid = false;
    };

  public:
    explicit DirectionalLight(const DirectionalLightProperties& props) : m_properties(props)
    {
        m_properties.colorIntensity.w = glm::max(m_properties.colorIntensity.w, 0.0f);

        EventManager::registerListeners(typeid(MainCameraChangedEvent).name(),
                                        [this](Ref<Event> e) { onMainCameraChanged(std::move(e)); });

        syncShadowCameras();
    }

    DirectionalLight() : DirectionalLight(DirectionalLightProperties{})
    {
        std::println("asdfghj");
    }

    void setDirection(glm::vec3 dir = {-1.f, -1.f, -1.f})
    {
        if (glm::length2(dir) > 1e-12f)
            m_properties.direction = glm::normalize(dir);
        syncShadowCameras();
    }

    void setColor(float r, float g, float b, float a)
    {
        m_properties.colorIntensity = {r, g, b, a};
        syncShadowCameras();
    }

    void setColor(glm::vec4 color)
    {
        m_properties.colorIntensity = color;
        syncShadowCameras();
    }

    void setIntensity(float I)
    {
        m_properties.colorIntensity.w = glm::max(I, 0.0f);
        syncShadowCameras();
    }

    [[nodiscard]] const DirectionalLightProperties& props() const
    {
        return m_properties;
    }

    [[nodiscard]] const std::array<std::optional<Camera>, kCascadeCount>& getShadowCams() const
    {
        return m_shadowCams;
    }

    DirectionalLightProperties getDirectionalLightProperties() const
    {
        return m_properties;
    }

    UUID getIdentifier() const
    {
        return identifier;
    }

  private:
    // Practical split scheme (λ∈[0,1]): mix of linear/log
    static inline void computeSplits(float zn, float zf, float (&splits)[kCascadeCount + 1], float lambda = 0.7f)
    {
        splits[0] = zn;
        splits[kCascadeCount] = zf;
        for (std::size_t i = 1; i < kCascadeCount; i++)
        {
            float si = float(i) / float(kCascadeCount);
            float zlin = zn + (zf - zn) * si;
            float zlog = zn * std::pow(zf / zn, si);
            splits[i] = glm::mix(zlin, zlog, lambda);
        }
    }

    // 8 NDC corners for a z-range in the main camera frustum → world space
    static inline void frustumCornersWS(const MainCamData& c, float z0, float z1, glm::vec3 out[8])
    {
        const glm::mat4 V = glm::lookAt(c.pos, c.pos + c.dir, c.up);
        const glm::mat4 P0 = glm::perspective(c.fovY, c.aspect, z0, z1);
        const glm::mat4 inv = glm::inverse(P0 * V);
        int k = 0;
        for (int iz = 0; iz < 2; ++iz)
        {
            float z = iz ? 1.f : -1.f;
            for (int iy = 0; iy < 2; ++iy)
            {
                float y = iy ? 1.f : -1.f;
                for (int ix = 0; ix < 2; ++ix)
                {
                    float x = ix ? 1.f : -1.f;
                    glm::vec4 p = inv * glm::vec4(x, y, z, 1);
                    out[k++] = glm::vec3(p) / p.w;
                }
            }
        }
    }

    // Fit ortho to points in light space
    static inline void aabb(const glm::vec3* pts, int n, glm::vec3& mn, glm::vec3& mx)
    {
        mn = mx = pts[0];
        for (int i = 1; i < n; i++)
        {
            mn = glm::min(mn, pts[i]);
            mx = glm::max(mx, pts[i]);
        }
    }

    void syncShadowCameras()
    {
        if (!m_main.valid)
            return; // wait until we saw the main cam once

        // 1) cascade splits
        float split[kCascadeCount + 1];
        computeSplits(m_main.zNear, m_main.zFar, split);

        // 2) light basis
        const glm::vec3 L = glm::normalize(m_properties.direction); // from light → scene
        const glm::vec3 worldUp(0.f, 1.f, 0.f);
        const glm::vec3 altUp(0.f, 0.f, 1.f);
        const float pole = 0.99f;
        const glm::vec3 up = (std::abs(glm::dot(L, worldUp)) > pole) ? altUp : worldUp;

        // Tunables
        const float mapRes = 1024.0f;   // shadow map resolution per cascade
        const float overlapXY = 1.02f;  // slight overlap between cascades in XY (2%)
        const float baseZPad = 10.0f;   // minimal z padding
        const float backoffMul = 1.5f;  // scale eye backoff by slice depth
        const float minBackoff = 50.0f; // do not let eye get too close

        for (std::size_t c = 0; c < kCascadeCount; ++c)
        {
            // ensure cam exists
            if (!m_shadowCams[c].has_value())
            {
                Camera::CameraProps p{};
                p.type = Camera::ECT_ORTHOGRAPHIC;
                m_shadowCams[c] = Camera(p);
            }

            // 3) corners of this cascade in WS (receiver slice)
            glm::vec3 cornersWS[8];
            frustumCornersWS(m_main, split[c], split[c + 1], cornersWS);

            // 4) Calculate bounding sphere of the frustum slice
            glm::vec3 centerWS(0.0f);
            for (int i = 0; i < 8; ++i)
                centerWS += cornersWS[i];
            centerWS /= 8.0f;

            float radius = 0.0f;
            for (int i = 0; i < 8; ++i)
                radius = glm::max(radius, glm::length(cornersWS[i] - centerWS));
            radius = std::ceil(radius * 16.0f) / 16.0f; // Round to avoid precision issues

            // 5) Create a stable view matrix
            // We use a fixed direction (L) and up vector, but we position the "camera"
            // such that it looks at the origin. However, to stabilize, we need to
            // ensure the projection window moves in texel-sized increments.

            // Transform center to light space
            const glm::vec3 zero(0.0f);
            const glm::mat4 Vlight = glm::lookAt(zero, zero + L, up);

            glm::vec3 centerLS = glm::vec3(Vlight * glm::vec4(centerWS, 1.0f));

            // 6) Texel snapping
            const float diameter = 2.0f * radius;
            const float texelSize = diameter / mapRes;

            centerLS.x = std::floor(centerLS.x / texelSize) * texelSize;
            centerLS.y = std::floor(centerLS.y / texelSize) * texelSize;

            // 7) Determine projection bounds
            // Since we use a bounding sphere, the ortho bounds are simply +/- radius
            // relative to the snapped center.
            const float left = centerLS.x - radius;
            const float right = centerLS.x + radius;
            const float bottom = centerLS.y - radius;
            const float top = centerLS.y + radius;

            // 8) Z range
            // We need to ensure we cover the slice depth plus potential casters in front.
            // In light space, the camera is at origin looking down +Z (or -Z depending on lookAt).
            // glm::lookAt(zero, zero+L, up) creates a view where forward is -Z (if standard GL).
            // Let's check: lookAt(eye, center, up). f = normalize(center - eye) = L.
            // s = normalize(cross(f, up)). u = cross(s, f).
            // Resulting view matrix transforms world to camera.
            // In camera space, forward is -Z. So L maps to -Z.

            // We need to find the min/max Z of the slice in light space to set near/far.
            // Actually, we can just use a large enough range centered on the slice.
            // But to be safe and simple:
            // The slice is contained within [centerLS.z - radius, centerLS.z + radius].
            // We extend the near plane (which is +Z in view space if we look down -Z? No wait).
            // Standard GL lookAt: forward is -Z.
            // So if L points into the scene, objects further away have more negative Z.

            // Let's just use a generous range relative to the center.
            // We want to capture casters between the light and the receiver.
            // So we extend "backwards" towards the light (positive Z in view space if L is -Z).

            float zMin = centerLS.z - radius - baseZPad - radius; // Extend back for casters
            float zMax = centerLS.z + radius + baseZPad;

            // 9) Program the camera
            if (m_shadowCams[c].has_value())
            {
                auto& cam = m_shadowCams[c];
                // We want the view matrix to be Vlight.
                // Camera::lookAt(target, up) sets position to current? No.
                // Camera::lookAt(target, up) sets target and up.
                // We need to set position and target such that the resulting view matrix is Vlight.
                // Vlight is lookAt(zero, zero + L, up).
                // So pos = zero, target = L.
                cam->setPosition(zero);
                cam->lookAt(zero + L, up);

                // Projection:
                // ortho(left, right, bottom, top, zNear, zFar)
                // In GL, zNear/zFar are distances from the camera.
                // If we are at 0, and looking down -Z (L direction).
                // Objects are at negative Z.
                // ortho projection maps -zNear to -1 and -zFar to +1.
                // Wait, glm::ortho(l, r, b, t, n, f) produces a matrix that maps
                // z=-n to -1 and z=-f to 1.
                // Our objects are in range [zMin, zMax] in view space.
                // Since view space Z is negative (looking down -Z), zMin/zMax are likely negative.
                // We need -near = zMax (closest to eye, most positive)
                // and -far = zMin (furthest from eye, most negative)
                // So near = -zMax, far = -zMin.

                cam->setOrthographic(left, right, bottom, top, -zMax, -zMin);
            }
        }
    }

    void onMainCameraChanged(Ref<Event> e)
    {
        auto ev = std::dynamic_pointer_cast<MainCameraChangedEvent>(e);
        if (!ev)
            return;

        // Expect your event to carry these (adapt names accordingly)
        m_main.pos = ev->position;
        m_main.dir = glm::normalize(ev->forward);
        m_main.up = ev->up;
        m_main.fovY = ev->fovY;
        m_main.aspect = ev->aspect;
        m_main.zNear = ev->znear;
        m_main.zFar = ev->zfar;
        m_main.valid = true;
        syncShadowCameras();
    }

  private:
    UUID identifier;
    DirectionalLightProperties m_properties;
    std::array<std::optional<Camera>, kCascadeCount> m_shadowCams;
    MainCamData m_main{};
};

} // namespace Engine
