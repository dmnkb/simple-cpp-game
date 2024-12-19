#pragma once

#include "EventManager.h"
#include "pch.h"
// DON'T CHANGE – INCLUDE ORDER MATTERS HERE
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>

struct WindowProps
{
    int width, height;
    const char* title;
    GLFWmonitor* monitor;
    GLFWwindow* share;
};

// TODO: Should be static to enforce implicit singleton pattern
class Window
{
  public:
    Window(const WindowProps& props, EventManager& eventManager);
    ~Window();

    bool isWindowOpen = true;

    GLFWwindow*& getNativeWindow()
    {
        return m_Window;
    }

    glm::vec2 getFrameBufferDimensions()
    {
        return m_frameBufferDimensions;
    }

  private:
    static Window* instance;

    GLFWwindow* m_Window;
    WindowProps m_WindowProps;

    glm::vec2 m_frameBufferDimensions = glm::vec2(0, 0);

    // Input
    EventManager& m_EventManager;
    bool m_FirstMosue = true;
    float m_CursorLastX, m_CursorLastY = 0.f;
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
};