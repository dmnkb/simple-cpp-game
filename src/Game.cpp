#include "Game.h"
#include "EventManager.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

// FIXME: to be updated on window resize
const int windowWidth = 640;
const int windowHeigth = 480;

WindowProps windowProps = {windowWidth, windowHeigth, "Simple CPP Game", NULL, NULL};
CameraProps camProps = {45.0f * (M_PI / 180.0f), ((float)windowWidth / windowHeigth), 0.1f, 100.0f};

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

    auto tex0 = TextureManager::loadTexture("assets/texture_02.png");
    tex0.bind(1);
    m_texture0 = tex0.id;
    auto tex1 = TextureManager::loadTexture("assets/texture_01.png");
    tex1.bind(2);
    m_texture1 = tex1.id;
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
    int previousDrawCalls = 0;
    int previousVertexCount = 0;
    int previousCubeCount = 0;

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

        // Clear the frame buffer and depth buffer
        glViewport(0, 0, m_Window.getFrameBufferDimensions().x, m_Window.getFrameBufferDimensions().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update scene
        m_Player.update(m_DeltaTime);
        m_EventManager.processEvents();

        // Render scene
        Renderer::beginScene(m_Camera);

        // Draw cubes
        if (i > 360)
            i = 0;
        i += .5 * m_DeltaTime;

        static const int count = 50;

        for (int y = -count / 2; y < count / 2; y++)
        {
            for (int x = -count / 2; x < count / 2; x++)
            {
                float distanceFromCenter = std::sqrt(x * x + y * y);         // Distance from the center in 2D
                float angle = i * (100 + std::sin(distanceFromCenter) * 50); // Sinusoidal variation
                Renderer::submitCube(glm::vec3(x, y, 0), glm::vec3(0, angle, 0), glm::vec3(.5),
                                     x % 2 == 0 ? m_texture0 : m_texture1);
            }
        }

        // End scene
        Renderer::endScene(m_Window.getNativeWindow());

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render ImGui elements
        static bool open = true;
        ImGui::Begin("Hello, ImGui!", &open);
        std::string fpsText = "FPS: " + std::to_string(fps);
        ImGui::Text("%s", fpsText.c_str());
        std::string drawCallsText = "Draw calls: " + std::to_string(previousDrawCalls);
        ImGui::Text("%s", drawCallsText.c_str());
        std::string vertCountText = "Vertices: " + std::to_string(previousVertexCount);
        ImGui::Text("%s", vertCountText.c_str());
        std::string cubeCountText = "Cubes: " + std::to_string(previousCubeCount);
        ImGui::Text("%s", cubeCountText.c_str());
        std::string roation = "Rotation: " + glm::to_string(m_Player.getRotation());
        ImGui::Text("%s", roation.c_str());
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(m_Window.getNativeWindow());
        glfwPollEvents();

        // Store the current stats for the next frame
        previousDrawCalls = Renderer::getStats().drawCalls;
        previousVertexCount = Renderer::getStats().vertexCount;
        previousCubeCount = Renderer::getStats().cubeCount;

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