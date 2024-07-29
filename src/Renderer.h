#pragma once

#include "Camera.h"
#include "Cube.h"
#include "EventManager.h"
#include "Shader.h"
#include "TextureManager.h"
#include "VertexArray.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
        uint32_t DrawCalls = 0;
        uint32_t VertexCount = 0;
    };

    static Statistics GetStats();
    static void ResetStats();

  private:
    static void startBatch();
    static void nextBatch();
    static void flush();
    static void draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount);
};