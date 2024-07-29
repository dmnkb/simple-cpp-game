#include "Game.h"
#include "EventManager.h"
#include "Renderer.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

// FIXME: to be updated on window resize
const int windowWidth = 640;
const int windowHeigth = 480;

WindowProps windowProps = {windowWidth, windowHeigth, "Simple CPP Game", NULL, NULL};
CameraProps camProps = {45.0f * (M_PI / 180.0f), ((float)windowWidth / windowHeigth), 0.1f, 100.0f, glm::vec3(5, 2, 5),
                        glm::vec3(0, 0, 0)};

Game::Game() : m_Window(windowProps, m_EventManager), m_Camera(camProps), m_Player(m_Camera, m_EventManager)
{
    Renderer::init();

    m_EventManager.registerListeners(typeid(KeyEvent).name(), [this](Event* event) { this->onKeyEvent(event); });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window.getNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Game::~Game()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

        glViewport(0, 0, m_Window.getFrameBufferDimensions().x, m_Window.getFrameBufferDimensions().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update scene
        m_Player.update(m_DeltaTime);
        m_EventManager.processEvents();

        // Render scene
        Renderer::beginScene(m_Camera);
        Renderer::ResetStats();

        // Things to be drawn
        Renderer::drawCube(glm::vec3(5, 0, 5), glm::vec3(0, 0, 0), glm::vec3(10, 0, 10));

        // Gui begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Gui render
        static bool open = true;

        ImGui::Begin("Hello, ImGui!", &open);
        std::string drawCallsText = "Draw calls: " + std::to_string(Renderer::GetStats().DrawCalls);
        std::cout << drawCallsText << std::endl;
        ImGui::Text("%s", drawCallsText.c_str());
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // End scene
        Renderer::endScene(m_Window.getNativeWindow());

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