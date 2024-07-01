#pragma once

#include "Camera.h"
#include "EventManager.h"

class Player
{
  public:
    Player(Camera& camera, EventManager& eventManager);

    void onKeyEvent(std::shared_ptr<Event> event);
    void onMousePosEvent(std::shared_ptr<Event> event);
    void update();

  private:
    Camera& m_Camera;
    vec3 m_Position;

    bool m_KeyW, m_KeyS;
};