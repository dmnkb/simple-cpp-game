#include "Plane.h"
#include "Camera.h"
#include <stdio.h>

Plane::Plane(const int width, const int height) : program(glCreateProgram()), ratio(width / (float)height)
{
  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint index_buffer;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
  vuv_location = glGetAttribLocation(program, "vUV");

  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, pos));
  glEnableVertexAttribArray(vuv_location);
  glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
}

void Plane::draw(const Camera &camera)
{
  mat4x4 m, p, v, vp, mvp;
  mat4x4_identity(m);

  camera.getProjectionMatrix(p);
  camera.getViewMatrix(v);

  mat4x4_mul(vp, p, v);
  mat4x4_mul(mvp, vp, m);

  glUseProgram(program);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
  glBindVertexArray(vertex_array);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw 2 triangles (6 vertices)
}
