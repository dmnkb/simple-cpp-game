#pragma once

#include "Camera.h"
#include "Player.h"
#include "Renderer.h"

class Game
{
  public:
    Game();
    ~Game();

    void run();
    void onKeyEvent(Event* event);

  private:
    Renderer m_Renderer;
    EventManager m_EventManager;
    Player m_Player;

    double m_DeltaTime, m_FPSUpdateTime;
    int m_FrameCount;

    std::shared_ptr<Cube> m_Cube;

    bool m_CanDisableCursor = true;
};