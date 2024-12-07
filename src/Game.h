#pragma once

#include "Camera.h"
#include "EventManager.h"
#include "Level.h"
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
    std::shared_ptr<Renderer> m_Renderer;
    EventManager m_EventManager;
    Camera m_Camera;
    Player m_Player;
    Scene m_scene;
    Level m_Level;

    GLuint m_texture0, m_texture1 = 0;

    double m_DeltaTime, m_FPSUpdateTime = 0.0;
    int m_FrameCount = 0;

    bool m_CanDisableCursor = true;
};