#include "Game.h"
#include "EventManager.h"

Game::Game()
    : m_Renderer(m_EventManager), m_Player(m_Camera, m_EventManager),
      m_Camera(45.0f * (M_PI / 180.0f), (float)m_Renderer.windowWidth / m_Renderer.windowHeight, 0.1f, 100.0f)
{
    m_Plane = new Plane(m_Renderer.windowWidth, m_Renderer.windowHeight);
}

void Game::run()
{
    while (m_Renderer.isWindowOpen)
    {
        m_Renderer.beginRender();

        auto deltaTime = m_Renderer.getDeltaTime();

        m_Plane->draw(m_Camera);
        m_Player.update(deltaTime);
        m_EventManager.processEvents();

        m_Renderer.endRender();
    }
}

Game::~Game()
{
    delete m_Plane;

    exit(EXIT_SUCCESS);
}