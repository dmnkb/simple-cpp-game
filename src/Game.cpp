#include "Game.h"
#include "EventManager.h"
#include "Renderer.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

// TODO: to be updated on window resize
const int windowWidth = 640;
const int windowHeigth = 480;

WindowProps windowProps = {windowWidth, windowHeigth, "Simple CPP Game", NULL, NULL};
CameraProps camProps = {45.0f * (M_PI / 180.0f), ((float)windowWidth / windowHeigth), 0.1f, 1000.0f};

Game::Game() : m_Window(windowProps, m_EventManager), m_Camera(camProps), m_Player(m_Camera, m_EventManager)
{
    Renderer::init();
    Scene::init();

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
    int previousDrawCalls = 0;
    int previousVertexCount = 0;
    double fps = 0.0;

    while (m_Window.isWindowOpen)
    {
        // Calculate delta time
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        m_DeltaTime = float(currentTime - lastTime);

        m_FrameCount++;
        if (currentTime - m_FPSUpdateTime >= 1.0)
        {
            fps = double(m_FrameCount) / (currentTime - m_FPSUpdateTime);
            m_FPSUpdateTime = currentTime;
            m_FrameCount = 0;
        }

        Renderer::beginScene(m_Camera);

        Scene::update();

        m_Player.update(m_DeltaTime);

        m_EventManager.processEvents();

        Renderer::update(m_Window.getFrameBufferDimensions());

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render ImGui elements
        static bool open = true;
        ImGui::Begin("Hello, ImGui!", &open);
        std::string fpsText = "FPS: " + std::to_string(fps);
        ImGui::Text("%s", fpsText.c_str());
        std::string drawCallsText = "Draw calls: " + std::to_string(Renderer::getStats().drawCallCount);
        ImGui::Text("%s", drawCallsText.c_str());
        std::string position = "Camera Position: " + glm::to_string(m_Player.getPosition());
        ImGui::Text("%s", position.c_str());
        std::string roation = "Camera Rotation: " + glm::to_string(m_Player.getRotation());
        ImGui::Text("%s", roation.c_str());
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events – TODO: move to Window class
        glfwSwapBuffers(m_Window.getNativeWindow());
        glfwPollEvents();

        // Reset stats for the next frame
        Renderer::resetStats();

        // Update last time for next frame
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