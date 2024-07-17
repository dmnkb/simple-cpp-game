#pragma once

#include "Camera.h"
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

    double m_DeltaTime, m_FPSUpdateTime;
    int m_FrameCount;

    std::shared_ptr<Cube> m_Cube;
};