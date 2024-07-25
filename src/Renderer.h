#pragma once

#include "Camera.h"
#include "Cube.h"
#include "EventManager.h"
#include "Shader.h"
#include "TextureManager.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Renderer
{
  public:
    Renderer(glm::vec2 frameBufferDimensions);
    ~Renderer();

    void render(Camera& m_Camera, GLFWwindow*& m_Window);

    // TODO:
    // An arbitrary mesh to be rendered without batching
    // addMesh()

    // TODO:
    // Called each frame. (What's supposed to be drawn is controlled by the actual cube entity)
    // Should update the batched cube vertex and index buffer to be drawn on render()
    // static void submitCube(position, rotation, scale, texture);

    std::shared_ptr<Cube> addCube(glm::vec3 position);

    // Will be obsolete once submitCube is in place, since the game / scene / level should keep track of
    // all entites which in turn decide what's submitted to the renderer.
    void removeCube(glm::vec3 position);

    bool isWindowOpen = false;

    // TODO:
    struct Statistics
    {
        uint32_t DrawCalls = 0;

        // uint32_t GetTotalVertexCount() const
        // {
        //     return cubeCount * 8;
        // }
        // uint32_t GetTotalIndexCount() const
        // {
        //     return cubeCount * 36;
        // }
    };
    static void ResetStats();
    static Statistics GetStats();

  private:
    // TODO:
    // batchCubes()
    // nextBatch()

    // static instance pointer to handle events
    static Renderer* instance;

    int m_FBWidth, m_FBHeight = 0;
    TextureManager m_TextureManager;
    std::shared_ptr<Shader> m_Shader;

    // TODO: remove
    std::vector<std::shared_ptr<Cube>> m_Cubes;
};