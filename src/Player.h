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

    glm::vec2 getRotation()
    {
        return m_Rotation;
    }

    void setIsCursorDisabled(bool disabled)
    {
        m_IsCursorDisabled = disabled;
    }

  private:
    Camera& m_Camera;
    glm::vec3 m_Position;
    glm::vec2 m_Rotation;
    glm::vec3 m_Direction;

    BoundingBox m_boundingBox;

    glm::vec2 m_camChange;
    glm::vec2 m_cursorPositionOld;
    std::vector<unsigned int> m_PressedKeys;
    std::deque<double> m_DeltaTimeHistory;
    bool m_IsCursorDisabled = true;
};