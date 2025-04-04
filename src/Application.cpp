#include "Application.h"
#include "EventManager.h"
#include "Renderer.h"
#include "Sandbox.h"
#include "Scene.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

Application::Application() : m_Player()
{
    // Window
    WindowProps windowProps = {
        .initialWidth = 640,
        .initialHeight = 480,
        .title = "Game",
    };
    Window::init(windowProps);

    // ImGUI
    initImGui();

    // Renderer
    m_renderer = CreateScope<Renderer>();

    // Scene
    CameraProps cameraProps = {45.0f * (M_PI / 180.0f), ((float)Window::dimensions.x / Window::dimensions.y), 0.1f,
                               1000.0f};
    m_Scene.init(cameraProps);

    // Events
    EventManager::registerListeners(typeid(KeyEvent).name(),
                                    [this](const Ref<Event> event) { this->onKeyEvent(event); });

    // Sandbox
    m_Sandbox = CreateScope<Sandbox>();
    m_Sandbox->init(m_Scene);
}

void Application::run()
{
    int previousDrawCalls = 0;
    int previousVertexCount = 0;
    double fps = 0.0;

    while (Window::open)
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

        m_Player.update(m_Scene, m_DeltaTime);

        m_renderer->update(m_Scene);

        m_Sandbox->update(m_DeltaTime);

        EventManager::processEvents();

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render ImGui elements
        static bool open = true;
        ImGui::Begin("Hello, ImGui!", &open, ImGuiWindowFlags_NoTitleBar);

        // FPS
        std::string fpsText = "FPS: " + std::to_string(fps);
        ImGui::Text("%s", fpsText.c_str());

        // Draw Calls
        // std::string drawCallsText = "Draw calls: " + std::to_string(Renderer::getStats().drawCallCount);
        // ImGui::Text("%s", drawCallsText.c_str());

        // Shadow Caster Depth Textures
        if (!m_Scene.getLightSceneNodes().empty())
        {
            ImGui::Separator();
            ImGui::Text("Shadow Depth Buffers:");
            for (size_t i = 0; i < m_Scene.getLightSceneNodes().size(); ++i)
            {
                const auto& texture = m_Scene.getLightSceneNodes()[i]->getDebugDepthTexture();
                ImGui::Image((void*)(intptr_t)texture->id, ImVec2(128, 96), ImVec2(1, 1), ImVec2(0, 0));
            }
        }

        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        Window::update();

        // Reset stats for the next frame
        // Renderer::resetStats();

        // Update last time for next frame
        lastTime = currentTime;
    }
}

void Application::onKeyEvent(const Ref<Event> event)
{
    auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(event);
    if (!keyEvent)
        return;

    if (keyEvent->key == GLFW_KEY_ESCAPE && keyEvent->action == GLFW_PRESS)
    {
        if (m_CanLockCursor)
        {
            Window::toggleCursorLock();
            m_CanLockCursor = false;
        }
    }
    if (keyEvent->key == GLFW_KEY_ESCAPE && keyEvent->action == GLFW_RELEASE)
    {
        m_CanLockCursor = true;
    }
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Window::shutdown();

    exit(EXIT_SUCCESS);
}

void Application::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(Window::glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}