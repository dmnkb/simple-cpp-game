#include "Renderer.h"
#include "Gui.h"
#include "TextureManager.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

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

Renderer::Renderer(glm::vec2 frameBufferDimensions)
    : m_FBWidth(frameBufferDimensions.x), m_FBHeight(frameBufferDimensions.y), m_TextureManager(),
      m_Shader(std::make_shared<Shader>(vertex_shader_text, fragment_shader_text))
{
    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {}

static std::vector<std::shared_ptr<Cube>> m_Cubes;

void Renderer::render(Camera& m_Camera, GLFWwindow*& m_Window)
{
    glViewport(0, 0, m_FBWidth, m_FBHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Gui::beginFrame();
    Gui::bind();

    // TODO:
    // Do this stuff for all batched vertex buffers
    // glBindVertexArray(m_VertexArray);
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);

    for (auto& cube : m_Cubes)
    {
        cube->draw(m_Camera);
    }

    Gui::render();

    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

std::shared_ptr<Cube> Renderer::addCube(glm::vec3 position)
{
    auto tex = TextureManager::loadTexture("assets/texture_02.png").id;
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