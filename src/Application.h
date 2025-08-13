#pragma once

#include "core/Core.h"
#include "core/EventManager.h"
#include "renderer/Renderer.h"
#include "scene/Player.h"
#include "scene/Sandbox.h"
#include "scene/Scene.h"

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