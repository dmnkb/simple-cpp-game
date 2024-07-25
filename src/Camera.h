#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraProps
{
    float fov, aspect, near, far;
    glm::vec3 defaultPosition, defaultTarget;
};

class Camera
{
  public:
    Camera(const CameraProps& props)
        : m_Fov(props.fov), m_Aspect(props.aspect), m_Near(props.near), m_Far(props.far),
          m_Position(props.defaultPosition), m_Target(props.defaultTarget){};

    void setPosition(glm::vec3 position)
    {
        m_Position = position;
    }

    void lookAt(glm::vec3 target)
    {
        m_Target = target;
    }

    glm::mat4 getProjectionMatrix() const
    {
        return glm::perspective(m_Fov, m_Aspect, m_Near, m_Far);
    }

    glm::mat4 getViewMatrix() const
    {
        glm::vec3 up = {0.0f, 1.0f, 0.0f};
        return glm::lookAt(m_Position, m_Target, up);
    }

  private:
    float m_Fov;
    float m_Aspect;
    float m_Near;
    float m_Far;

    glm::vec3 m_Position;
    glm::vec3 m_Target;
};