#pragma once

#include "EventManager.h"
#include "Player.h"
#include "Renderer.h"
#include "Sandbox.h"
#include "Scene.h"
#include "core/Core.h"

namespace Engine
{

class Application
{
  public:
    Application();
    ~Application();

    void run();
    void onKeyEvent(const Ref<Event> event);

  private:
    void initImGui();

  private:
    Scope<Renderer> m_renderer;
    Scene m_Scene;
    Player m_Player;

    double m_DeltaTime = 0.0;
    double m_FPSUpdateTime = 0.0;
    int m_FrameCount = 0;
    bool m_CanLockCursor = true;

    Scope<Sandbox> m_Sandbox;
};

} // namespace Engine