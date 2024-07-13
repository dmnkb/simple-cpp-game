#pragma once

#define GLFW_INCLUDE_NONE
#include "EventManager.h"
#include "Plane.h"
#include "TextureManager.h"
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

    Texture loadTexture(const char* path)
    {
        return m_TextureManager.loadTexture(path);
    };

    // Placeholder functions
    void addPlane(glm::vec3& position);
    void removePlane(glm::vec3& position);

    bool isWindowOpen = false;

  private:
    GLFWwindow* m_Window;
    EventManager& m_EventManager;
    TextureManager m_TextureManager;

    static Renderer* instance;

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mousePosCallback(GLFWwindow* window, double xpos, double ypos);

    int m_WindowWidth, m_WindowHeight = 0;
    bool m_FirstMosue = true;
    float m_CursorLastX, m_CursorLastY = 0.f;

    // placeholder storage for planes
    std::vector<Plane> m_Planes;
};