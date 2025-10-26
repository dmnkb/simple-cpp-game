#pragma once

#include "core/Event.h"
#include "core/EventManager.h"
#include "pch.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{

class Camera
{
  public:
    enum ECameraType
    {
        ECT_PROJECTION,
        ECT_ORTHOGRAPHIC
    };

    struct CameraProps
    {
        // Store radians for fov; default 45 deg
        float fov = glm::radians(45.0f);
        float aspect = 4.0f / 3.0f;
        float near = 0.1f;
        float far = 1000.0f;

        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 target{0.0f, 0.0f, -1.0f}; // looking down -Z by default
        glm::vec3 up{0.0f, 1.0f, 0.0f};      // NEW: configurable up vector

        ECameraType type = ECT_PROJECTION;

        // (Optional) orthographic bounds; used only if type == ECT_ORTHOGRAPHIC
        float orthoLeft = -20.0f;
        float orthoRight = 20.0f;
        float orthoBottom = -20.0f;
        float orthoTop = 20.0f;

        // Controls if the aspect ratio adjusts with the screen red
        bool isMainCamera = false;
    };
    Camera() : m_props{} {}

    Camera(const CameraProps& props) : m_props(props)
    {
        EventManager::registerListeners(typeid(WindowReziseEvent).name(),
                                        [this](const Ref<Event> event) { this->onFramebufferReziseEvent(event); });
    }

    void isMainCamera()
    {
        m_props.isMainCamera = true;
    }

    void setPosition(const glm::vec3& position)
    {
        if (m_props.position != position)
        {
            m_props.position = position;
            onUpdate();
        }
    }

    void lookAt(const glm::vec3& target)
    {
        if (m_props.target != target)
        {
            m_props.target = target;
            onUpdate();
        }
    }

    void lookAt(const glm::vec3& target, const glm::vec3& up)
    {
        m_props.target = target;
        glm::vec3 u = up;
        float len2 = glm::dot(u, u);
        if (len2 < 1e-12f)
            u = glm::vec3(0.0f, 1.0f, 0.0f);

        if (m_props.up != u)
        {
            m_props.up = u * glm::inversesqrt(glm::max(len2, 1e-12f));
            onUpdate();
        }
    }

    void setDirection(const glm::vec3& dir)
    {
        glm::vec3 d = dir;

        float len2 = glm::dot(d, d);
        if (len2 < 1e-12f)
            d = glm::vec3(0.0f, 0.0f, -1.0f);
        else
            d = d * glm::inversesqrt(len2);

        auto newTarget = m_props.position + d;

        if (newTarget != m_props.target)
        {
            m_props.target = newTarget;
            onUpdate();
        }
    }

    void setDirection(const glm::vec3& dir, const glm::vec3& upHint)
    {
        // normalize dir (fallback to -Z)
        glm::vec3 d = dir;
        float d2 = glm::dot(d, d);
        d = (d2 > 1e-12f) ? d * glm::inversesqrt(d2) : glm::vec3(0.0f, 0.0f, -1.0f);

        // normalize up (fallback to world-up)
        glm::vec3 up = upHint;
        float u2 = glm::dot(up, up);
        up = (u2 > 1e-12f) ? up * glm::inversesqrt(u2) : glm::vec3(0.0f, 1.0f, 0.0f);

        // avoid parallel up
        if (glm::abs(glm::dot(d, up)) > 0.999f)
            up = (glm::abs(d.y) < 0.999f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

        // re-orthonormalize
        const glm::vec3 right = glm::normalize(glm::cross(d, up));
        up = glm::normalize(glm::cross(right, d));

        auto newTarget = m_props.position + d;
        auto newUp = up;

        if (newTarget != m_props.target || newUp != m_props.up)
        {
            m_props.target = newTarget;
            m_props.up = newUp;
            onUpdate();
        }
    }

    void setPerspective(float fovYRadians, float aspect, float nearPlane, float farPlane)
    {
        m_props.type = ECT_PROJECTION;
        m_props.fov = std::max(0.001f, std::min(fovYRadians, glm::radians(179.0f)));
        m_props.aspect = std::max(1e-6f, aspect);
        m_props.near = std::max(1e-6f, nearPlane);
        m_props.far = std::max(m_props.near + 1e-6f, farPlane);
    }

    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
    {
        m_props.type = ECT_ORTHOGRAPHIC;
        m_props.orthoLeft = left;
        m_props.orthoRight = right;
        m_props.orthoBottom = bottom;
        m_props.orthoTop = top;
        m_props.near = nearPlane;
        m_props.far = farPlane;
    }

    glm::mat4 getProjectionMatrix() const
    {
        if (m_props.type == ECT_ORTHOGRAPHIC)
        {
            return glm::ortho(m_props.orthoLeft, m_props.orthoRight, m_props.orthoBottom, m_props.orthoTop,
                              m_props.near, m_props.far);
        }
        return glm::perspective(m_props.fov, m_props.aspect, m_props.near, m_props.far);
    }

    glm::mat4 getViewMatrix() const
    {
        glm::vec3 up = m_props.up;
        float len2 = glm::dot(up, up);
        if (len2 < 1e-12f)
            up = glm::vec3(0.0f, 1.0f, 0.0f);
        else
            up = up * glm::inversesqrt(len2);
        return glm::lookAt(m_props.position, m_props.target, up);
    }

    glm::vec3 getPosition() const
    {
        return m_props.position;
    }

    glm::vec3 getDirection() const
    {
        return glm::normalize(m_props.target - m_props.position);
    }

    float getAspect() const
    {
        return m_props.aspect;
    }

  private:
    void onFramebufferReziseEvent(const Ref<Event> event)
    {
        if (!m_props.isMainCamera)
            return;

        auto windowReziseEvent = std::dynamic_pointer_cast<WindowReziseEvent>(event);
        if (!windowReziseEvent)
            return;

        const float width = static_cast<float>(windowReziseEvent->windowWidth);
        const float height = static_cast<float>(windowReziseEvent->windowHeight);

        m_props.aspect = width / height;
    }

    void onUpdate()
    {
        // Queue change event
        if (!m_props.isMainCamera)
            return;

        // Forward from position → target
        glm::vec3 fwd = m_props.target - m_props.position;
        float f2 = glm::dot(fwd, fwd);
        fwd = (f2 > 1e-12f) ? fwd * glm::inversesqrt(f2) : glm::vec3(0, 0, -1);

        // Safe, normalized up
        glm::vec3 up = m_props.up;
        float u2 = glm::dot(up, up);
        up = (u2 > 1e-12f) ? up * glm::inversesqrt(u2) : glm::vec3(0, 1, 0);

        // Perspective params (fallback FOV if orthographic)
        float fovY = (m_props.type == ECT_PROJECTION) ? m_props.fov : glm::radians(60.0f);
        float aspect = m_props.aspect;
        float zn = m_props.near;
        float zf = m_props.far;

        auto ev = CreateRef<MainCameraChangedEvent>(m_props.position, fwd, up, fovY, aspect, zn, zf);
        EventManager::queueEvent(ev);
    }

    CameraProps m_props;
};

} // namespace Engine
