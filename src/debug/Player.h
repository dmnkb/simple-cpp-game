#pragma once

#include "core/Core.h"
#include "core/EventManager.h"
#include "renderer/Camera.h"
#include "util/SmoothDamp.h"
#include <glm/glm.hpp>

namespace Engine
{

class Player
{
  public:
    Player();

    void onKeyEvent(const Ref<Event> event);
    void onMouseMoveEvent(const Ref<Event> event);
    void update(Camera& camera, double deltaTime);
    bool isKeyPressed(unsigned int key);

    glm::vec3 getPosition()
    {
        return m_Position;
    }
    glm::vec2 getRotation()
    {
        return m_Rotation;
    }

  private:
    // Core player attributes
    glm::vec3 m_Position = glm::vec3({-85, 50, -80});
    glm::vec2 m_Rotation = glm::vec2({45, -15}); // yaw (x), pitch (y)
    glm::vec3 m_Direction = glm::vec3(0.f);
    float m_verticalVelocity = 0;

    glm::vec3 m_TargetPosition = m_Position; // where gameplay wants us to be
    glm::vec3 m_PositionVel = glm::vec3(0);  // SmoothDamp velocity accumulator

    // Camera
    glm::vec2 m_camChange = glm::vec2(0, 0);
    glm::vec2 m_cursorPositionOld;

    // Input handling
    std::vector<unsigned int> m_PressedKeys;

    // Performance
    int m_collisionPairCheckCount = 0;
};

} // namespace Engine
