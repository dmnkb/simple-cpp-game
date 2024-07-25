#include "Game.h"
#include "EventManager.h"
#include "Gui.h"
#include "Window.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// FIXME: to be updated on window resize
const int windowWidth = 640;
const int windowHeigth = 480;

WindowProps props = {windowWidth, windowHeigth, "Simple CPP Game", NULL, NULL};
Game::Game()
    : m_Window(props, m_EventManager), m_Camera(45.0f * (M_PI / 180.0f), ((float)windowWidth / windowHeigth), 0.1f,
                                                100.0f, glm::vec3(5, 2, 5), glm::vec3(0, 0, 0)),
      m_Renderer(m_Window.getFrameBufferDimensions()), m_Player(m_Camera, m_EventManager)
{
    m_EventManager.registerListeners(typeid(KeyEvent).name(), [this](Event* event) { this->onKeyEvent(event); });
    m_Cube = m_Renderer.addCube(glm::vec3(5, 0, 5));
    m_Cube->setScale(glm::vec3(10, 1, 10));

    Gui& gui = Gui::getInstance(m_Window.getNativeWindow());
    std::shared_ptr<GuiText> myGuiText = std::make_shared<GuiText>("Foo");
    Gui::pushElement("foo", myGuiText);
}

Game::~Game()
{
    exit(EXIT_SUCCESS);
}

void Game::run()
{
    float i = 0.f;

    while (m_Window.isWindowOpen)
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
            glfwSetWindowTitle(m_Window.getNativeWindow(), title.c_str());
        }

        // Update scene
        m_Player.update(m_DeltaTime);
        m_EventManager.processEvents();

        // Render scene
        m_Renderer.render(m_Camera, m_Window.getNativeWindow());

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;
    }
}

void Game::onKeyEvent(Event* event)
{
    auto keyEvent = dynamic_cast<KeyEvent*>(event);
    if (!keyEvent)
        return;

    // lock cursor
    if (keyEvent->key == GLFW_KEY_ESCAPE && keyEvent->action == GLFW_PRESS)
    {
        int currentMode = glfwGetInputMode(m_Window.getNativeWindow(), GLFW_CURSOR);
        m_CanDisableCursor = false;
        if (currentMode == GLFW_CURSOR_DISABLED)
        {
            glfwSetInputMode(m_Window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_Player.setIsCursorDisabled(true);

            int windowWidth, windowHeight;
            glfwGetWindowSize(m_Window.getNativeWindow(), &windowWidth, &windowHeight);
            glfwSetCursorPos(m_Window.getNativeWindow(), windowWidth / 2, windowHeight / 2);
        }
        else
        {
            glfwSetInputMode(m_Window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_Player.setIsCursorDisabled(false);
        }
    }
    if (keyEvent->key == GLFW_KEY_ESCAPE && keyEvent->action == GLFW_RELEASE)
    {
        m_CanDisableCursor = true;
    }
}