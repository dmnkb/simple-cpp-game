#include "Game.h"
#include "EventManager.h"
#include <GLFW/glfw3.h>

Game::Game()
    : m_Renderer(m_EventManager), m_Player(m_Camera, m_EventManager),
      m_Camera(45.0f * (M_PI / 180.0f), m_Renderer.getViewport().x / m_Renderer.getViewport().y, 0.1f, 100.0f),
      m_DeltaTime(0.f), m_FPSUpdateTime(0.f), m_FrameCount(0)
{
}

Game::~Game()
{
    exit(EXIT_SUCCESS);
}

void Game::run()
{
    while (m_Renderer.isWindowOpen)
    {
        // Calculate delta time
        // glfwGetTime is called only once, the first time this function is called
        static double lastTime = glfwGetTime();

        double currentTime = glfwGetTime();
        m_DeltaTime = float(currentTime - lastTime);

        // Print FPS every second
        m_FrameCount++;
        if (currentTime - m_FPSUpdateTime >= 1.0)
        {
            double fps = double(m_FrameCount) / (currentTime - m_FPSUpdateTime);

            m_FPSUpdateTime = currentTime;
            m_FrameCount = 0;

            std::string title = "FPS: " + std::to_string(fps);
            glfwSetWindowTitle(m_Renderer.getWindow(), title.c_str());
        }

        // Begin rendering
        m_Renderer.beginRender();

        // Update scene
        m_Plane.draw(m_Camera);
        m_Player.update(m_DeltaTime);
        m_EventManager.processEvents();

        // End rendering
        m_Renderer.endRender();

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;
    }
}