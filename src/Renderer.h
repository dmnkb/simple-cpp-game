#pragma once

#define GLFW_INCLUDE_NONE
#include "EventManager.h"
#include <GLFW/glfw3.h>

class Renderer
{
  public:
    Renderer(EventManager& eventManager);
    ~Renderer();

    void beginRender();
    void endRender();

    GLFWwindow* getWindow();
    double getDeltaTime();

    bool isWindowOpen;
    int windowWidth, windowHeight;

  private:
    GLFWwindow* m_Window;
    EventManager& m_EventManager;
    static Renderer* instance;

    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void handleKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
    void handleMousePosCallback(GLFWwindow* window, double xpos, double ypos);
    static void closeCallback(GLFWwindow* window);

    double m_DeltaTime, m_PreviousTime, m_FPSUpdateTime;
    int m_FrameCount;
};