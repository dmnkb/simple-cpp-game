#pragma once

#include "AABB.h"
#include "Camera.h"
#include "EventManager.h"
#include "Level.h"
#include "pch.h"
#include <glm/glm.hpp>

class Player
{
  public:
    Player(Camera& camera, EventManager& eventManager);

    void onKeyEvent(Event* event);
    void onMouseMoveEvent(Event* event);
    void update(double deltaTime, Level& level);
    bool isKeyPressed(unsigned int key);

    glm::vec3 getPosition()
    {
        return m_Position;
    }
    glm::vec2 getRotation()
    {
        return m_Rotation;
    }

    void setIsCursorDisabled(bool disabled)
    {
        m_IsCursorDisabled = disabled;
    }

  private:
    // Core player attributes
    glm::vec3 m_Position = glm::vec3({0, 20, 0});
    glm::vec2 m_Rotation = glm::vec2({45, -20});
    glm::vec3 m_Direction = glm::vec3(0.f);
    BoundingBox m_boundingBox;
    bool m_onGround = false;
    float m_verticalVelocity = 0;

    // Camera
    Camera& m_Camera;
    glm::vec2 m_camChange;
    glm::vec2 m_cursorPositionOld;
    bool m_IsCursorDisabled = true;

    // Input handling
    std::vector<unsigned int> m_PressedKeys;

    // Performance/Timing
    std::deque<double> m_DeltaTimeHistory;
};