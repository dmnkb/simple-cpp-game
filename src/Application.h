#pragma once

#include "EventManager.h"
#include "Player.h"
#include "Renderer.h"

class Application
{
  public:
    Application();
    ~Application();

    void run();
    void onKeyEvent(Event* event);

  private:
    void initImGui();

  private:
    Scope<Renderer> m_renderer;
    Player m_Player;

    double m_DeltaTime, m_FPSUpdateTime = 0.0;
    int m_FrameCount = 0;

    bool m_CanDisableCursor = true;
};