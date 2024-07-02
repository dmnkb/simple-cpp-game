#pragma once

#include "Camera.h"
#include "EventManager.h"

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
    vec3 m_Position;
    vec2 m_Rotation;
    struct
    {
        double x;
        double y;
    } m_camChange, m_cursorPositionOld;

    std::vector<unsigned int> m_PressedKeys;
};