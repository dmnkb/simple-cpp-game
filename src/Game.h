#pragma once

#include "Camera.h"
#include "EventManager.h"
#include "Player.h"
#include "Renderer.h"
#include "Window.h"

class Game
{
  public:
    Game();
    ~Game();

    void run();
    void onKeyEvent(Event* event);

  private:
    EventManager m_EventManager;
    Window m_Window;
    Camera m_Camera;
    Renderer m_Renderer;
    Player m_Player;

    double m_DeltaTime, m_FPSUpdateTime = 0.0;
    int m_FrameCount = 0;

    std::shared_ptr<Cube> m_Cube;

    bool m_CanDisableCursor = true;
};