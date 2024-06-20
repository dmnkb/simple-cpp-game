#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <linmath.h>

#include "Triangle.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "Camera.h"

static void error_callback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods)
{
  printf("Key pressed: %d\n", key);

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
  printf("Hello, Triangle!\n");
  fflush(stdout); // Ensure the output is flushed immediately
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  Camera camera(45.0f * (3.14159265f / 180.0f), (float)width / height, 0.1f,
                10.0f);

  Triangle *Tri1 = new Triangle(width, height);
  Triangle *Tri2 = new Triangle(width * 2, height * 2);

  while (!glfwWindowShouldClose(window))
  {

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Tri1->draw(camera);
    Tri2->draw(camera);

    camera.setPosition(0, sin((float)glfwGetTime()) * 2, 2);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  delete Tri1;
  delete Tri2;

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
