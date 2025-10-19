#pragma once
#include "Camera.h"
#include "core/Event.h"
#include "core/EventManager.h"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

namespace Engine
{

inline constexpr std::size_t kCascadeCount = 4;

class DirectionalLight
{
  public:
    struct DirectionalLightProperties
    {
        glm::vec3 direction = glm::normalize(glm::vec3(-1.f, -1.f, -1.f)); // from light → scene
        glm::vec4 colorIntensity = {1, 1, 1, 10};
    };

    // --- main camera snapshot we need to build cascades ---
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
    DirectionalLight() : DirectionalLight(DirectionalLightProperties{}) {}

    void setDirection(glm::vec3 dir = {-1.f, -1.f, -1.f})
    {
        if (glm::length2(dir) > 1e-12f)
            m_properties.direction = glm::normalize(dir);
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
    [[nodiscard]] const std::array<Ref<Camera>, kCascadeCount>& getShadowCams() const
    {
        return m_shadowCams;
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
        const glm::vec3 up =
            (std::abs(glm::dot(L, glm::vec3(0, 1, 0))) > 0.99f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);

        for (std::size_t c = 0; c < kCascadeCount; c++)
        {
            // ensure cam exists
            if (!m_shadowCams[c])
            {
                Camera::CameraProps p{};
                p.type = Camera::ECT_ORTHOGRAPHIC;
                m_shadowCams[c] = CreateRef<Camera>(p);
            }

            // 3) corners of this cascade in WS
            glm::vec3 cornersWS[8];
            frustumCornersWS(m_main, split[c], split[c + 1], cornersWS);

            // cascade center
            glm::vec3 center(0);
            for (auto& v : cornersWS)
                center += v;
            center *= 1.0f / 8.0f;

            // 4) build light view at some distance "behind" the cascade
            float backoff = 100.0f; // big enough to cover near plane; tweak later
            glm::vec3 eye = center - L * backoff;
            glm::mat4 Vlight = glm::lookAt(eye, center, up);

            // 5) AABB in light space → ortho bounds
            glm::vec3 cornersLS[8];
            for (int i = 0; i < 8; i++)
            {
                glm::vec4 p = Vlight * glm::vec4(cornersWS[i], 1);
                cornersLS[i] = glm::vec3(p);
            }
            glm::vec3 mn, mx;
            aabb(cornersLS, 8, mn, mx);

            // optional stabilize (snap xy to texel grid)
            const float mapRes = 2048.0f;
            float texel = (mx.x - mn.x) / mapRes;
            glm::vec2 centerXY = 0.5f * glm::vec2(mn.x + mx.x, mn.y + mx.y);
            centerXY = glm::floor(centerXY / texel) * texel;
            float halfW = 0.5f * (mx.x - mn.x), halfH = 0.5f * (mx.y - mn.y);
            mn.x = centerXY.x - halfW;
            mx.x = centerXY.x + halfW;
            mn.y = centerXY.y - halfH;
            mx.y = centerXY.y + halfH;

            // small margin
            const float m = 1.0f;
            mn -= glm::vec3(m, m, 0);
            mx += glm::vec3(m, m, 0);

            // 6) set camera params (ortho bounds + near/far in light space)
            Camera& cam = *m_shadowCams[c];
            cam.setOrthographic(mn.x, mx.x, mn.y, mx.y, -mx.z, -mn.z); // note: depends on your camera convention

            cam.setPosition(eye);
            cam.setDirection(L); // looks along light → scene
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
    DirectionalLightProperties m_properties;
    std::array<Ref<Camera>, kCascadeCount> m_shadowCams;
    MainCamData m_main{};
};

} // namespace Engine
