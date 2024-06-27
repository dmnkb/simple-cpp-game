#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "Camera.h"
#include <GLFW/glfw3.h>
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
                                        "in vec2 vPos;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
                                        "}\n";

static const char *fragment_shader_text = "#version 330\n"
                                          "out vec4 fragment;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    fragment = vec4(1.0, 1.0, 1.0, 1.0);\n"
                                          "}\n";

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
};