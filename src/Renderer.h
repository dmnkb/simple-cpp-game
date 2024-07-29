#pragma once

#include "Camera.h"
#include "Cube.h"
#include "EventManager.h"
#include "Shader.h"
#include "TextureManager.h"
#include "VertexArray.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct CubeVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexIndex;
};

struct SceneData
{
    glm::mat4 ViewProjectionMatrix;
};

struct Statistics
{
    uint32_t DrawCalls = 0;
};

struct RendererData
{
    static const uint32_t MaxCubes;
    static const uint32_t MaxVertices;
    static const uint32_t MaxIndices;

    std::shared_ptr<VertexArray> CubeVertexArray;
    std::shared_ptr<VertexBuffer> CubeVertexBuffer;

    uint32_t CubeIndexCount = 0;
    CubeVertex* CubeVertexBufferBase = nullptr;
    CubeVertex* CubeVertexBufferPtr = nullptr;

    uint32_t TextureSlotIndex = 1; // Start at 1 because 0 is reserved for the default white texture
    std::array<std::shared_ptr<Texture>, 32> TextureSlots;

    Statistics Stats;
};

class Renderer
{
  public:
    Renderer(glm::vec2 frameBufferDimensions);
    ~Renderer();

    static void beginScene(Camera& camera);
    static void endScene(GLFWwindow*& window);

    static void drawCube(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    bool isWindowOpen = false;

    static Statistics GetStats();
    static void ResetStats();

  private:
    static void startBatch();
    static void nextBatch();
    static void flush();
    static void draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount);

    TextureManager m_TextureManager;
    static int m_FBWidth, m_FBHeight;
    static std::shared_ptr<Shader> m_Shader;

    static std::unique_ptr<SceneData> s_SceneData;
    static RendererData s_Data;
};