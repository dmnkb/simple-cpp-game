#pragma once

#include "Camera.h"
#include "EventManager.h"
#include "Shader.h"
#include "TextureManager.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

static const char* vertex_shader_text = "#version 330 core\n"
                                        "layout(location = 0) in vec3 a_Position;\n"
                                        "layout(location = 1) in vec2 a_UV;\n"
                                        "uniform mat4 u_ViewProjection;\n"
                                        "out vec2 UV;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);\n"
                                        "    UV = a_UV;\n"
                                        "}\n";

static const char* fragment_shader_text = "#version 330\n"
                                          "in vec2 UV;\n"
                                          "out vec4 fragment;\n"
                                          "uniform sampler2D myTextureSampler;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    vec4 texColor = texture(myTextureSampler, UV);\n"
                                          "    fragment = mix(vec4(1.0, 0.0, 0.0, 1.0), texColor, 1.0);\n"
                                          "}\n";

class Renderer
{
  public:
    static void init();
    static void shutdown();

    static void beginScene(Camera& camera);
    static void endScene(GLFWwindow*& window);

    static void drawCube(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    bool isWindowOpen = false;

    struct Statistics
    {
        uint32_t drawCalls = 0;
        uint32_t vertexCount = 0;
        uint32_t cubeCount = 0;
    };

    static Statistics getStats();
    static void resetStats();

    static glm::mat4 getVPM();

  private:
    static void startBatch();
    static void nextBatch();
    static void flush();
    static void draw();
};