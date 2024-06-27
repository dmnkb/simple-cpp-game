#include "Renderer.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

Renderer *Renderer::instance = nullptr;

Renderer::Renderer() : isWindowOpen(true), keyW(false), keyA(false), keyS(false), keyD(false)
{
  instance = this;

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);

  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, keyCallback);
  glfwSetWindowCloseCallback(window, closeCallback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
}

void Renderer::beginRender()
{

  glViewport(0, 0, windowWidth, windowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endRender()
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void Renderer::errorCallback(int error, const char *description) { fprintf(stderr, "Error: %s\n", description); }

void Renderer::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (instance)
  {
    instance->handleKeyCallback(window, key, scancode, action, mods);
  }
}

void Renderer::closeCallback(GLFWwindow *window) { instance->isWindowOpen = false; }

void Renderer::handleKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  printf("Key pressed: %d\n", key);
  printf("Action: %d\n", action);

  keyW = action > 0 && key == 87;
  keyA = action > 0 && key == 65;
  keyS = action > 0 && key == 83;
  keyD = action > 0 && key == 68;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    isWindowOpen = false;
  }
}

Renderer::~Renderer()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}
