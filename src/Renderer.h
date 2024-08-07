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
                                        "layout(location = 2) in vec3 a_Normal;\n"
                                        "uniform mat4 u_ViewProjection;\n"
                                        "out vec2 v_UV;\n"
                                        "out vec3 FragPos;\n"
                                        "out vec3 v_Normal;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    v_UV = a_UV;\n"
                                        "    v_Normal = normalize(a_Normal);\n"
                                        "    FragPos = a_Position;\n"
                                        "    gl_Position = u_ViewProjection * vec4(FragPos, 1.0);\n"
                                        "}\n";

static const char* fragment_shader_text = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "in vec2 v_UV;\n"
                                          "in vec3 v_Normal;\n"
                                          "in vec3 FragPos;\n"
                                          "uniform sampler2D myTextureSampler;\n"
                                          "uniform vec3 viewPos;\n"
                                          "uniform vec3 lightPos;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    vec3 norm = normalize(v_Normal);\n"
                                          "    vec3 lightDir = normalize(lightPos - FragPos);\n"
                                          "    float diff = max(dot(norm, lightDir), 0.0);\n"
                                          "    vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
                                          "    vec3 ambient = vec3(0.5, 0.5, 0.5);\n"
                                          "    vec3 diffuse = diff * lightColor;\n"
                                          "    vec3 lighting = ambient + diffuse;\n"
                                          "    vec4 textureColor = texture(myTextureSampler, v_UV);\n"
                                          "    FragColor = vec4(lighting, 1.0) * textureColor;\n"
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