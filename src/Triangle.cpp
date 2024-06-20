#include "Triangle.h"
#include <stdio.h>

Triangle::Triangle() : program(glCreateProgram())
{
  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);

  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  mvp_location = glGetUniformLocation(program, "MVP");
  vpos_location = glGetAttribLocation(program, "vPos");
  vcol_location = glGetAttribLocation(program, "vCol");

  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, pos));
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, col));
}

void Triangle::draw(const float ratio)
{
  mat4x4 m, p, mvp;
  mat4x4_identity(m);
  mat4x4_rotate_Z(m, m, (float)glfwGetTime());
  mat4x4_translate_in_place(m, 0.0f, 0.0f, -2 - sin(glfwGetTime()));

  // Create a perspective projection matrix
  float fov = 45.0f * (3.14159265f / 180.0f); // Field of view in radians
  float near = 0.1f;                          // Near clipping plane
  float far = 10.0f;                          // Far clipping plane
  float aspect = ratio;                       // Aspect ratio

  mat4x4_perspective(p, fov, aspect, near, far);
  mat4x4_mul(mvp, p, m);

  glUseProgram(program);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
  glBindVertexArray(vertex_array);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}
