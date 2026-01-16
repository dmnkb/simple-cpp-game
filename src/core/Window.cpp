#include "core/Window.h"
#include "core/Core.h"
#include "renderer/GLDebug.h"

namespace Engine
{
// TODO: Consider making Window a class so that the application can have multiple windows.
namespace Window
{

GLFWwindow* glfwWindow;
bool open = false;
bool firstMosue = true;
float cursorLastX = 0.0f;
float cursorLastY = 0.0f;
glm::vec2 dimensions = glm::vec2(1080, 720);
glm::vec2 frameBufferDimensions = glm::vec2(0, 0);

void init(const WindowProps& props)
{
    glfwSetErrorCallback([](int error, const char* description) { fprintf(stderr, "Error: %s\n", description); });

    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindow = glfwCreateWindow(props.initialWidth, props.initialHeight, props.title, NULL, NULL);

    if (!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(glfwWindow, keyCallback);
    glfwSetCursorPosCallback(glfwWindow, mousePosCallback);
    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* window) { open = false; });

    glfwSetFramebufferSizeCallback(glfwWindow, framebufferSizeCallback);

    glfwMakeContextCurrent(glfwWindow);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);

    int fbWidth, fbHeight = 0;
    glfwGetFramebufferSize(glfwWindow, &fbWidth, &fbHeight);
    assert((fbWidth > 0 && fbHeight > 0) && "[ERROR] Framebuffer size appears to be zero.");
    frameBufferDimensions = glm::vec2(fbWidth, fbHeight);

    open = true;
}

void shutdown()
{
    if (!glfwWindow) return;  // Already shut down
    
    open = false;
    glfwDestroyWindow(glfwWindow);
    glfwWindow = nullptr;
    glfwTerminate();
}

void pollEvents()
{
    glfwPollEvents();
}

void swapBuffers()
{
    glfwSwapBuffers(glfwWindow);
}

double getElapsedTime()
{
    return glfwGetTime();
}

void toggleCursorLock()
{
    int currentMode = glfwGetInputMode(glfwWindow, GLFW_CURSOR);

    glfwSetInputMode(glfwWindow, GLFW_CURSOR,
                     currentMode == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    if (currentMode == GLFW_CURSOR_DISABLED)
    {
        glfwSetCursorPos(glfwWindow, dimensions.x / 2, dimensions.y / 2);
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto event = CreateRef<KeyEvent>(key, action);
    EventManager::queueEvent(event);
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMosue)
    {
        cursorLastX = static_cast<float>(xpos);
        cursorLastY = static_cast<float>(ypos);
        firstMosue = false;
    }

    float xoffset = cursorLastX - xpos;
    float yoffset = cursorLastY - ypos;

    auto event =
        CreateRef<MouseMoveEvent>(xoffset, yoffset, glfwGetInputMode(glfwWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED);
    EventManager::queueEvent(event);

    cursorLastX = xpos;
    cursorLastY = ypos;
}

void framebufferSizeCallback(GLFWwindow* window, int fbW, int fbH)
{
    assert((fbW > 0 && fbH > 0) &&
           "[ERROR] (framebufferSizeCallback) Framebuffer size appears to be zero after resizing.");

    frameBufferDimensions = {float(fbW), float(fbH)};

    GLCall(glViewport(0, 0, fbW, fbH));

    auto ev = CreateRef<WindowReziseEvent>(fbW, fbH);
    EventManager::queueEvent(ev);
}

} // namespace Window
} // namespace Engine