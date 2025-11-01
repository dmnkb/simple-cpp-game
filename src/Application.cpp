#include "Application.h"
#include "core/EventManager.h"
#include "core/Profiler.h"
#include "core/Window.h"
#include "debug/DebugUI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pch.h"
#include "renderer/ClearColor.h"
#include "renderer/Renderer.h"
#include "scene/Sandbox.h"
#include "scene/Scene.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Engine
{

Application::Application() : m_Player()
{
    // Window
    Window::WindowProps windowProps = {
        .initialWidth = 1080,
        .initialHeight = 720,
        .title = "Game",
    };
    Window::init(windowProps);

    // ImGUI
    initImGui();

    // Renderer
    m_renderer = CreateScope<Renderer>();

    // Scene
    Camera::CameraProps cameraProps = {};
    cameraProps.fov = glm::radians(45.0f);
    cameraProps.aspect = Window::dimensions.x / Window::dimensions.y;
    cameraProps.near = 0.1f;
    cameraProps.far = 1000.0f;
    cameraProps.isMainCamera = true;

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

        Profiler::resetStats();

        m_Player.update(m_Scene, m_DeltaTime);

        m_renderer->update(m_Scene);

        m_Sandbox->update(m_DeltaTime);

        EventManager::processEvents();

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DebugUI::render(fps, m_Scene);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        Window::update();

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

    // Fonts (optional)
    io.Fonts->AddFontFromFileTTF("assets/fonts/Inter_24pt-Regular.ttf", 14.0f);
    // io.Fonts->AddFontFromFileTTF("assets/fonts/alagard.ttf", 14.0f);
    // io.Fonts->AddFontFromFileTTF("assets/fonts/Romulus.ttf", 13.f);

    ImGuiStyle& style = ImGui::GetStyle();

    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.06f, 0.54f, 0.91f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.06f, 0.54f, 0.91f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.04f, 0.48f, 0.82f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    colors[ImGuiCol_Separator] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // General
    style.WindowPadding = ImVec2(12.000000, 12.000000);
    style.FramePadding = ImVec2(4.000000, 4.000000);
    style.ItemSpacing = ImVec2(4.000000, 4.000000);
    style.ItemInnerSpacing = ImVec2(4.000000, 4.000000);
    style.TouchExtraPadding = ImVec2(0.000000, 0.000000);
    style.IndentSpacing = 22.000000;
    style.ScrollbarSize = 12.000000;
    style.GrabMinSize = 12.000000;
    style.WindowRounding = 4.000000;
    style.FrameRounding = 2.000000;
    style.PopupRounding = 0.000000;
    style.ScrollbarRounding = 9.000000;
    style.GrabRounding = 2.000000;
    style.TabRounding = 2.000000;
    style.TabBarBorderSize = 0.000000;
    style.WindowBorderSize = 0.000000;
    style.SeparatorTextBorderSize = 1.000000;

    // Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(Window::glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

} // namespace Engine
