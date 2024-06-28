#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <linmath.h>

typedef struct Vertex
{
    vec3 pos;
    vec2 uv;
} Vertex;

typedef struct Index
{
    vec3 pos;
} Index;

static const Vertex vertices[4] = {
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}}, // top left
    {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},  // top right
    {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}}, // bottom right
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}} // bottom left
};

static const GLuint indices[6] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

static const char *vertex_shader_text = "#version 330\n"
                                        "uniform mat4 MVP;\n"
                                        "in vec3 vPos;\n"
                                        "in vec2 vUV;\n"
                                        "out vec2 UV;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    gl_Position = MVP * vec4(vPos, 1.0);\n"
                                        "    UV = vUV;\n"
                                        "}\n";

static const char *fragment_shader_text = "#version 330\n"
                                          "in vec2 UV;\n"
                                          "out vec4 fragment;\n"
                                          "uniform sampler2D myTextureSampler;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    vec4 texColor = texture(myTextureSampler, UV);\n"
                                          "    fragment = texColor;\n"
                                          "}\n";

static void checkShaderCompilation(GLuint shader)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        fprintf(stderr, "Error compiling shader: %s\n", infoLog);
    }
}

static void checkProgramLinking(GLuint program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        fprintf(stderr, "Error linking program: %s\n", infoLog);
    }
}

class Plane
{
  public:
    Plane(const int width, const int height);

    void draw(const Camera &camera);

  private:
    const GLuint program;
    GLint mvp_location;
    GLint vpos_location;
    GLint vcol_location;
    GLint vuv_location;
    GLuint vertex_array;

    float ratio;

    GLuint texture;
};