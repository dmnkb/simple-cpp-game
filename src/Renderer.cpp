#include "Renderer.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(EventManager& eventManager) : m_EventManager(eventManager), isWindowOpen(true)
{
    instance = this;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);

    if (!m_Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(m_Window, keyCallback);
    glfwSetCursorPosCallback(m_Window, mousePosCallback);
    glfwSetWindowCloseCallback(m_Window, closeCallback);

    glfwMakeContextCurrent(m_Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glfwGetFramebufferSize(m_Window, &windowWidth, &windowHeight);
}

void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (instance)
        instance->handleKeyCallback(window, key, scancode, action, mods);
}

void Renderer::handleKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
    if (instance)
        instance->handleMousePosCallback(window, xpos, ypos);
}

void Renderer::handleMousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    MousePosEvent* event = new MousePosEvent(xpos, ypos);
    m_EventManager.queueEvent(event);
}

void Renderer::closeCallback(GLFWwindow* window)
{
    instance->isWindowOpen = false;
}

void Renderer::beginRender()
{
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endRender()
{
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

GLFWwindow* Renderer::getWindow()
{
    return m_Window;
}

void Renderer::errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

Renderer::~Renderer()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}
