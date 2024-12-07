#pragma once

#include "Camera.h"
#include "EventManager.h"
#include "Light.h"
#include "Shader.h"
#include "TextureManager.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Renderer
{
  public:
    static void init();
    static void shutdown();

    static void beginScene(Camera& camera);
    static void endScene(GLFWwindow*& window);

    static void submitCube(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::float32 textureID,
                           float tilingAmount = 1);
    static void submitLights(const std::vector<Light>& lights);

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