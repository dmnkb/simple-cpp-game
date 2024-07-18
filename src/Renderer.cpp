#include "Renderer.h"
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <iostream>
#include <stdlib.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// FIXME: SHould be updated on window resize event
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

static const char* vertex_shader_text = "#version 330\n"
                                        "uniform mat4 MVP;\n"
                                        "in vec3 vPos;\n"
                                        "in vec2 vUV;\n"
                                        "out vec2 UV;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    gl_Position = MVP * vec4(vPos, 1.0);\n"
                                        "    UV = vUV;\n"
                                        "}\n";

static const char* fragment_shader_text = "#version 330\n"
                                          "in vec2 UV;\n"
                                          "out vec4 fragment;\n"
                                          "uniform sampler2D myTextureSampler;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    vec4 texColor = texture(myTextureSampler, UV);\n"
                                          "    fragment = texColor;\n"
                                          "}\n";

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(EventManager& eventManager)
    : m_Camera(45.0f * (M_PI / 180.0f), ((float)WINDOW_WIDTH / WINDOW_HEIGHT), 0.1f, 100.0f, glm::vec3(5, 2, 5),
               glm::vec3(0, 0, 0)),
      m_EventManager(eventManager), isWindowOpen(true), m_TextureManager()
{
    glfwSetErrorCallback([](int error, const char* description) { fprintf(stderr, "Error: %s\n", description); });

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Simple CPP Game", NULL, NULL);

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

    // Disable for now to test imgui
    // glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(m_Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glfwGetFramebufferSize(m_Window, &m_WindowWidth, &m_WindowHeight);

    m_Shader = new Shader(vertex_shader_text, fragment_shader_text);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

Renderer::~Renderer()
{
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Renderer::render()
{
    glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create ImGui window
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text("This is a simple ImGui window.");
    ImGui::End();

    for (auto& cube : m_Cubes)
    {
        cube->draw(m_Camera);
    }

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    // Don't propagate any events for now
    return;

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

std::shared_ptr<Cube> Renderer::addCube(glm::vec3 position)
{
    auto tex = loadTexture("assets/texture_02.png").id;
    auto cube = std::make_shared<Cube>(tex, m_Shader, position);
    m_Cubes.push_back(cube);
    return cube;
}

void Renderer::removeCube(glm::vec3 position)
{
    for (auto it = m_Cubes.begin(); it != m_Cubes.end();)
    {
        (*it)->remove();
        it = m_Cubes.erase(it);
    }
}