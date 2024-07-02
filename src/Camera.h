#pragma once
#include <glm/glm.hpp>

class Camera
{
  public:
    Camera(float fov, float aspect, float near, float far, glm::vec3 defaultTarget = glm::vec3(0, 0, 0),
           glm::vec3 defaultRotation = glm::vec3(0, 0, 0));

    void setPosition(glm::vec3 position);
    void lookAt(glm::vec3 target);

    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;

  private:
    float fov;
    float aspect;
    float near;
    float far;

    glm::vec3 m_Position;
    glm::vec3 m_Target;
};