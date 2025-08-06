#pragma once

#include "EventManager.h"
#include "pch.h"
// DON'T CHANGE – INCLUDE ORDER MATTERS HERE
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>

namespace Engine
{

namespace Window
{

struct WindowProps
{
    const int initialWidth;
    const int initialHeight;
    const char* title;
};

void init(const WindowProps& props);
void update();
void shutdown();
double getElapsedTime();
void toggleCursorLock();

extern GLFWwindow* glfwWindow;
extern bool open;
extern bool firstMosue;
extern float cursorLastX;
extern float cursorLastY;
extern glm::vec2 frameBufferDimensions;
extern glm::vec2 dimensions;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void resizeCallback(GLFWwindow* window, int width, int height);

}; // namespace Window

} // namespace Engine