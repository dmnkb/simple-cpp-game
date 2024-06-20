#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Renderer
{
public:
  Renderer();
  ~Renderer();

  void beginRender();
  void endRender();

  bool isWindowOpen;
  int windowWidth, windowHeight;

private:
  GLFWwindow *window;

  static Renderer *instance;

  static void error_callback(int error, const char *description);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);

  void handleKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                         int mods);
};