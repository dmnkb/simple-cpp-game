#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "editor/ImGuiLayer.h"

namespace Engine
{

void ImGuiLayer::onAttach(GLFWwindow* window)
{
    if (m_initialized || !window) return;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Enable docking if you like; you can toggle this later if needed
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable docking TODO:

    // Setup Dear ImGui style
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
    colors[ImGuiCol_Border] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
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
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.06f, 0.54f, 0.91f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.06f, 0.54f, 0.91f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.06f, 0.54f, 0.91f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.54f, 0.91f, 1.00f);
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
    style.IndentSpacing = 0.000000;

    // Setup Platform/Renderer backends
    // Use a GLSL version compatible with your OpenGL context (Mac: 410 core is fine)
    const char* glsl_version = "#version 410";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    m_initialized = true;
}

void ImGuiLayer::onDetach()
{
    if (!m_initialized) return;

    // Shutdown backends
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    // Destroy ImGui context
    ImGui::DestroyContext();

    m_initialized = false;
}

void ImGuiLayer::beginFrame()
{
    if (!m_initialized || !m_enabled) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::endFrame()
{
    if (!m_initialized || !m_enabled) return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // If you later enable multi-viewport support (ImGuiConfigFlags_ViewportsEnable),
    // you'll also want to update and render additional platform windows here.
    // For now, we keep it simple and only render to the main framebuffer.
}

} // namespace Engine
