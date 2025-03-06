#include "Window.h"

static WindowData s_windowData;

void Window::init(const WindowProps& props)
{
    s_windowData.windowProps = props;

    glfwSetErrorCallback([](int error, const char* description) { fprintf(stderr, "Error: %s\n", description); });

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_windowData.window = glfwCreateWindow(props.width, props.height, "Simple CPP Game", NULL, NULL);

    if (!s_windowData.window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(s_windowData.window, keyCallback);
    glfwSetCursorPosCallback(s_windowData.window, mousePosCallback);

    glfwSetWindowCloseCallback(s_windowData.window, [](GLFWwindow* window) { s_windowData.isWindowOpen = false; });

    glfwMakeContextCurrent(s_windowData.window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);

    // TODO: Add resize event

    int fbWidth, fbHeight = 0;
    glfwGetFramebufferSize(s_windowData.window, &fbWidth, &fbHeight);
    s_windowData.frameBufferDimensions = glm::vec2(fbWidth, fbHeight);
    s_windowData.isWindowOpen = true;
}

void Window::shutdown()
{
    glfwDestroyWindow(s_windowData.window);
    glfwTerminate();
}

bool Window::getIsWindowOpen()
{
    return s_windowData.isWindowOpen;
}

void Window::swapBuffers()
{
    glfwSwapBuffers(s_windowData.window);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

double Window::getElapsedTime()
{
    return glfwGetTime();
}

GLFWwindow*& Window::getNativeWindow()
{
    return s_windowData.window;
}

glm::vec2 Window::getFrameBufferDimensions()
{
    return s_windowData.frameBufferDimensions;
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    KeyEvent* event = new KeyEvent(key, action);
    EventManager::queueEvent(event);
}

void Window::mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (s_windowData.firstMosue)
    {
        s_windowData.cursorLastX = static_cast<float>(xpos);
        s_windowData.cursorLastY = static_cast<float>(ypos);
        s_windowData.firstMosue = false;
    }

    float xoffset = s_windowData.cursorLastX - xpos;
    float yoffset = s_windowData.cursorLastY - ypos;

    MouseMoveEvent* event = new MouseMoveEvent(xoffset, yoffset);
    EventManager::queueEvent(event);

    s_windowData.cursorLastX = xpos;
    s_windowData.cursorLastY = ypos;
}