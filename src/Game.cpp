#include "Game.h"
#include "EventManager.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Game::Game()
    : m_Renderer(m_EventManager), m_Player(m_Renderer.getCamera(), m_EventManager), m_DeltaTime(0.f),
      m_FPSUpdateTime(0.f), m_FrameCount(0)
{
    auto myCube = m_Renderer.addCube(glm::vec3(0, 0, 0));
    myCube->setScale(glm::vec3(10, .5f, 10));
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

        // Update scene
        m_Player.update(m_DeltaTime);
        m_EventManager.processEvents();

        // Render scene
        m_Renderer.render();

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;
    }
}