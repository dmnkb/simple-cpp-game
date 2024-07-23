#pragma once

#include "Camera.h"
#include "Cube.h"
#include "EventManager.h"
#include "Shader.h"
#include "TextureManager.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Renderer
{
  public:
    Renderer(EventManager& eventManager);
    ~Renderer();

    void render();

    GLFWwindow* getWindow()
    {
        return m_Window;
    }

    Texture loadTexture(const char* path)
    {
        return m_TextureManager.loadTexture(path);
    };

    Camera& getCamera()
    {
        return m_Camera;
    }

    std::shared_ptr<Cube> addCube(glm::vec3 position);
    void removeCube(glm::vec3 position);

    bool isWindowOpen = false;

  private:
    GLFWwindow* m_Window;
    EventManager& m_EventManager;
    TextureManager m_TextureManager;

    // static instance pointer to handle events
    static Renderer* instance;

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mousePosCallback(GLFWwindow* window, double xpos, double ypos);

    int m_FBWidth, m_FBHeight = 0;
    bool m_FirstMosue = true;
    float m_CursorLastX, m_CursorLastY = 0.f;

    Camera m_Camera;
    std::shared_ptr<Shader> m_Shader;
    std::vector<std::shared_ptr<Cube>> m_Cubes;
};