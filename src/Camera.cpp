#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string.h>

Camera::Camera(float fov, float aspect, float near, float far, glm::vec3 defaultPosition, glm::vec3 defaultTarget)
    : fov(fov), aspect(aspect), near(near), far(far), m_Position(defaultPosition), m_Target(defaultTarget){};

void Camera::setPosition(glm::vec3 position)
{
    m_Position = position;
}

void Camera::lookAt(glm::vec3 target)
{
    m_Target = target;
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(fov, aspect, near, far);
}

glm::mat4 Camera::getViewMatrix() const
{
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    return glm::lookAt(m_Position, m_Target, up);
}
