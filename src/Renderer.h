#pragma once

#define GLFW_INCLUDE_NONE
#include "EventManager.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Renderer
{
  public:
    Renderer(EventManager& eventManager);
    ~Renderer();

    void beginRender();
    void endRender();

    GLFWwindow* getWindow()
    {
        return m_Window;
    }
    const glm::vec2 getViewport()
    {
        return glm::vec2(m_WindowWidth, m_WindowHeight);
    }

    bool isWindowOpen = false;

  private:
    GLFWwindow* m_Window;
    EventManager& m_EventManager;
    static Renderer* instance;

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mousePosCallback(GLFWwindow* window, double xpos, double ypos);

    int m_WindowWidth, m_WindowHeight = 0;
    bool m_FirstMosue = true;
    float m_CursorLastX, m_CursorLastY = 0.f;
};