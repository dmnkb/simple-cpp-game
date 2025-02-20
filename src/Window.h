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

struct WindowData
{
    GLFWwindow* window;
    WindowProps windowProps;
    glm::vec2 frameBufferDimensions = glm::vec2(0, 0);

    bool isWindowOpen = false;
    bool firstMosue = true;
    float cursorLastX, cursorLastY = 0.f;
};

class Window
{
  public:
    static void init(const WindowProps& props);
    static void shutdown();
    static bool getIsWindowOpen();
    static void swapBuffers();
    static void pollEvents();
    static double getElapsedTime();

    static GLFWwindow*& getNativeWindow();
    static glm::vec2 getFrameBufferDimensions();

  private:
    static Window* instance;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
};