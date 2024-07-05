#pragma once

#include "Camera.h"
#include "Plane.h"
#include "Player.h"
#include "Renderer.h"

class Game
{
  public:
    Game();

    void run();

    ~Game();

  private:
    Renderer m_Renderer;
    EventManager m_EventManager;
    Player m_Player;
    Camera m_Camera;

    Plane m_Plane;

    double m_DeltaTime, m_FPSUpdateTime;
    int m_FrameCount;
};