#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum ECameraType
{
    ECT_PROJECTION,
    ECT_ORTHOGRAPHIC
};

struct CameraProps
{
    float fov = 45.0f * (M_PI / 180.0f);
    float aspect = 4 / 3;
    float near = 0.1f;
    float far = 1000.0f;
    glm::vec3 position, target = glm::vec3(0, 0, 0);
    ECameraType type = ECT_PROJECTION;
};

class Camera
{
  public:
    Camera(const CameraProps& props) : m_props(props) {}

    void setPosition(glm::vec3 position)
    {
        m_props.position = position;
    }

    void lookAt(glm::vec3 target)
    {
        m_props.target = target;
    }

    glm::mat4 getProjectionMatrix() const
    {
        if (m_props.type == ECT_ORTHOGRAPHIC)
        {
            float orthoLeft = -20.0f;
            float orthoRight = 20.0f;
            float orthoBottom = -20.0f;
            float orthoTop = 20.0f;
            return glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_props.near, m_props.far);
        }
        return glm::perspective(m_props.fov, m_props.aspect, m_props.near, m_props.far);
    }

    glm::mat4 getViewMatrix() const
    {
        glm::vec3 up = {0.0f, 1.0f, 0.0f};
        return glm::lookAt(m_props.position, m_props.target, up);
    }

    glm::vec3 getPosition() const
    {
        return m_props.position;
    }

  private:
    CameraProps m_props;
};