#pragma once

#include "Camera.h"
#include "EventManager.h"
#include "Player.h"
#include "Renderer.h"
#include "Scene.h"
#include "Window.h"

class Game
{
  public:
    Game();
    ~Game();

    void run();
    void onKeyEvent(Event* event);

  private:
    Window m_Window;
    EventManager m_EventManager;
    Camera m_Camera;
    Player m_Player;

    double m_DeltaTime, m_FPSUpdateTime = 0.0;
    int m_FrameCount = 0;

    bool m_CanDisableCursor = true;
};