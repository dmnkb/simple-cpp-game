#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
  public:
    Camera(float fov, float aspect, float near, float far, glm::vec3 defaultPosition, glm::vec3 defaultTarget)
        : fov(fov), aspect(aspect), near(near), far(far){};

    // clang-format off
    void setPosition(glm::vec3 position) { m_Position = position; }
    void lookAt(glm::vec3 target) { m_Target = target; }

    glm::mat4 getProjectionMatrix() const { return glm::perspective(fov, aspect, near, far); }
    // clang-format on

    glm::mat4 getViewMatrix() const
    {
        glm::vec3 up = {0.0f, 1.0f, 0.0f};
        return glm::lookAt(m_Position, m_Target, up);
    }

  private:
    float fov;
    float aspect;
    float near;
    float far;

    glm::vec3 m_Position;
    glm::vec3 m_Target;
};