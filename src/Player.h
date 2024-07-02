#pragma once

#include "Camera.h"
#include "EventManager.h"
#include <glm/glm.hpp>

class Player
{
  public:
    Player(Camera& camera, EventManager& eventManager);

    void onKeyEvent(Event* event);
    void onMousePosEvent(Event* event);
    void update();
    bool isKeyPressed(unsigned int key);

  private:
    Camera& m_Camera;
    glm::vec3 m_Position;
    glm::vec2 m_Rotation;
    glm::vec3 m_Direction;
    glm::vec2 m_camChange;
    glm::vec2 m_cursorPositionOld;

    std::vector<unsigned int> m_PressedKeys;
};