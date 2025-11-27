#pragma once

#include "Camera.h"
#include "core/Event.h"
#include "core/EventManager.h"
#include "util/uuid.h"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

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
        const float mapRes = 2048.0f;   // shadow map resolution per cascade
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

            // slice center in world
            glm::vec3 centerWS(0.0f);
            for (int i = 0; i < 8; ++i)
                centerWS += cornersWS[i];
            centerWS *= 1.0f / 8.0f;

            // 4) build light view behind the slice
            const float sliceDepth = (split[c + 1] - split[c]);
            const float backoff = glm::max(minBackoff, backoffMul * sliceDepth);
            const glm::vec3 eyeWS = centerWS - L * backoff;
            const glm::mat4 Vlight = glm::lookAt(eyeWS, centerWS, up);

            // 5) transform slice corners into light space and get AABB
            glm::vec3 cornersLS[8];
            for (int i = 0; i < 8; ++i)
                cornersLS[i] = glm::vec3(Vlight * glm::vec4(cornersWS[i], 1.0f));

            glm::vec3 mn = cornersLS[0], mx = cornersLS[0];
            for (int i = 1; i < 8; ++i)
            {
                mn = glm::min(mn, cornersLS[i]);
                mx = glm::max(mx, cornersLS[i]);
            }

            // 6) rotation-invariant XY fit (enclose with a circle → square)
            glm::vec3 centerLS = 0.5f * (mn + mx);

            float maxR = 0.0f;
            for (int i = 0; i < 8; ++i)
            {
                glm::vec2 d = glm::vec2(cornersLS[i].x, cornersLS[i].y) - glm::vec2(centerLS.x, centerLS.y);
                maxR = glm::max(maxR, glm::length(d));
            }
            maxR *= overlapXY;

            // 7) texel-grid stabilization (snap center in LS XY)
            const float texel = (2.0f * maxR) / mapRes;
            if (texel > 0.0f)
            {
                glm::vec2 cxy(centerLS.x, centerLS.y);
                cxy = glm::floor(cxy / texel) * texel;
                centerLS.x = cxy.x;
                centerLS.y = cxy.y;
            }

            // Ortho XY from stabilized center + half-extent
            const float left = centerLS.x - maxR;
            const float right = centerLS.x + maxR;
            const float bottom = centerLS.y - maxR;
            const float top = centerLS.y + maxR;

            // 8) Z range (include potential casters!)
            //    Start from true slice z span, then extend by caster margin ~ XY radius,
            //    plus extra padding for near cascade.
            float zmin = mn.z;
            float zmax = mx.z;

            // caster margin: anything within the cascade footprint can cast onto it
            float casterExtend = maxR; // try 1.0–2.0 * maxR if needed
            float zPad = baseZPad;

            // give the near cascade more headroom (most likely to miss nearby casters)
            if (c == 0)
            {
                casterExtend *= 1.5f;
                zPad = glm::max(zPad, 0.5f * sliceDepth);
            }

            zmin -= (casterExtend + zPad);
            zmax += (casterExtend + zPad);

            // 9) program the camera with the SAME view we used for fitting
            if (m_shadowCams[c].has_value())
            {
                auto& cam = m_shadowCams[c];
                cam->setPosition(eyeWS);
                cam->lookAt(centerWS, up);                     // match Vlight exactly
                cam->setOrthographic(left, right, bottom, top, // projection
                                     -zmax, -zmin);            // keep your convention
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
