#include "Window.h"

Window* Window::instance = nullptr;

Window::Window(const WindowProps& props, EventManager& eventManager)
    : m_WindowProps(props), m_EventManager(eventManager)
{
    instance = this;

    glfwSetErrorCallback([](int error, const char* description) { fprintf(stderr, "Error: %s\n", description); });

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(props.width, props.height, "Simple CPP Game", NULL, NULL);

    if (!m_Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

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

    glfwMakeContextCurrent(m_Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    int fbWidth, fbHeight = 0;
    glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
    m_frameBufferDimensions = glm::vec2(fbWidth, fbHeight);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    KeyEvent* event = new KeyEvent(key, action);
    m_EventManager.queueEvent(event);
}

void Window::mousePosCallback(GLFWwindow* window, double xpos, double ypos)
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