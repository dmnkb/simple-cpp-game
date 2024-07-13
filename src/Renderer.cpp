#include "Renderer.h"
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(EventManager& eventManager) : m_EventManager(eventManager), isWindowOpen(true), m_TextureManager()
{
    glfwSetErrorCallback([](int error, const char* description) { fprintf(stderr, "Error: %s\n", description); });

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(640, 480, "Simple CPP Game", NULL, NULL);

    if (!m_Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    instance = this;
    glfwSetKeyCallback(m_Window,
                       [](GLFWwindow* window, int key, int scancode, int action, int mods)
                       {
                           if (instance)
                               instance->keyCallback(window, key, scancode, action, mods);
                       });
    glfwSetCursorPosCallback(m_Window,
                             [](GLFWwindow* window, double xpos, double ypos)
                             {
                                 if (instance)
                                     instance->mousePosCallback(window, xpos, ypos);
                             });
    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) { instance->isWindowOpen = false; });

    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(m_Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glfwGetFramebufferSize(m_Window, &m_WindowWidth, &m_WindowHeight);
}

Renderer::~Renderer()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Renderer::beginRender()
{
    glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endRender()
{
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        isWindowOpen = false;
    }

    KeyEvent* event = new KeyEvent(key, action);
    m_EventManager.queueEvent(event);
}

void Renderer::mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (m_FirstMosue)
    {
        m_CursorLastX = static_cast<float>(xpos);
        m_CursorLastY = static_cast<float>(ypos);
        m_FirstMosue = false;
    }

    float xoffset = m_CursorLastX - xpos;
    float yoffset = m_CursorLastY - ypos;

    MouseMoveEvent* event = new MouseMoveEvent(xoffset, yoffset);
    m_EventManager.queueEvent(event);

    m_CursorLastX = xpos;
    m_CursorLastY = ypos;
}

void Renderer::addPlane(glm::vec3& position) {}
void Renderer::removePlane(glm::vec3& position) {}