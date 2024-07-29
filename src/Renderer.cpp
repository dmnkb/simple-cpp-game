#include "Renderer.h"
#include "TextureManager.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

static const char* vertex_shader_text = "#version 330\n"
                                        "uniform mat4 u_ViewProjection;\n"
                                        "uniform vec3 a_Position;\n"
                                        "in vec3 vPos;\n"
                                        "in vec2 vUV;\n"
                                        "out vec2 UV;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);\n"
                                        "    UV = vUV;\n"
                                        "}\n";

static const char* fragment_shader_text = "#version 330\n"
                                          "in vec2 UV;\n"
                                          "out vec4 fragment;\n"
                                          "uniform sampler2D myTextureSampler;\n"
                                          "void main()\n"
                                          "{\n"
                                          //   "    vec4 texColor = texture(myTextureSampler, UV);\n"
                                          "    vec4 texColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
                                          "    fragment = texColor;\n"
                                          "}\n";

int Renderer::m_FBWidth = 0;
int Renderer::m_FBHeight = 0;
std::shared_ptr<Shader> Renderer::m_Shader = nullptr;
std::unique_ptr<SceneData> Renderer::s_SceneData = std::make_unique<SceneData>();
RendererData Renderer::s_Data;

const uint32_t RendererData::MaxCubes = 20000;
const uint32_t RendererData::MaxVertices = RendererData::MaxCubes * 8;
const uint32_t RendererData::MaxIndices = RendererData::MaxCubes * 36;

uint32_t cubeIndices[] = {
    // Front face
    0, 1, 2, 2, 3, 0,
    // Back face
    4, 5, 6, 6, 7, 4,
    // Left face
    4, 0, 3, 3, 7, 4,
    // Right face
    1, 5, 6, 6, 2, 1,
    // Top face
    3, 2, 6, 6, 7, 3,
    // Bottom face
    4, 5, 1, 1, 0, 4};

Renderer::Renderer(glm::vec2 frameBufferDimensions) : m_TextureManager()
{
    m_FBWidth = frameBufferDimensions.x;
    m_FBHeight = frameBufferDimensions.y;
    m_Shader = std::make_shared<Shader>(vertex_shader_text, fragment_shader_text);

    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    s_Data.CubeVertexArray = std::make_shared<VertexArray>();
    s_Data.CubeVertexBuffer = std::make_shared<VertexBuffer>(s_Data.MaxVertices * sizeof(CubeVertex));
    s_Data.CubeVertexArray->addVertexBuffer(s_Data.CubeVertexBuffer);

    // Define the layout manually
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, TexCoord));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, TexIndex));
    glEnableVertexAttribArray(2);

    s_Data.CubeVertexBufferBase = new CubeVertex[s_Data.MaxIndices];

    uint32_t* cubeIndices = new uint32_t[s_Data.MaxIndices];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_Data.MaxIndices; i += 36)
    {
        // Front face
        cubeIndices[i + 0] = offset + 0;
        cubeIndices[i + 1] = offset + 1;
        cubeIndices[i + 2] = offset + 2;
        cubeIndices[i + 3] = offset + 2;
        cubeIndices[i + 4] = offset + 3;
        cubeIndices[i + 5] = offset + 0;
        // Back face
        cubeIndices[i + 6] = offset + 4;
        cubeIndices[i + 7] = offset + 5;
        cubeIndices[i + 8] = offset + 6;
        cubeIndices[i + 9] = offset + 6;
        cubeIndices[i + 10] = offset + 7;
        cubeIndices[i + 11] = offset + 4;
        // Left face
        cubeIndices[i + 12] = offset + 4;
        cubeIndices[i + 13] = offset + 0;
        cubeIndices[i + 14] = offset + 3;
        cubeIndices[i + 15] = offset + 3;
        cubeIndices[i + 16] = offset + 7;
        cubeIndices[i + 17] = offset + 4;
        // Right face
        cubeIndices[i + 18] = offset + 1;
        cubeIndices[i + 19] = offset + 5;
        cubeIndices[i + 20] = offset + 6;
        cubeIndices[i + 21] = offset + 6;
        cubeIndices[i + 22] = offset + 2;
        cubeIndices[i + 23] = offset + 1;
        // Top face
        cubeIndices[i + 24] = offset + 3;
        cubeIndices[i + 25] = offset + 2;
        cubeIndices[i + 26] = offset + 6;
        cubeIndices[i + 27] = offset + 6;
        cubeIndices[i + 28] = offset + 7;
        cubeIndices[i + 29] = offset + 3;
        // Bottom face
        cubeIndices[i + 30] = offset + 4;
        cubeIndices[i + 31] = offset + 5;
        cubeIndices[i + 32] = offset + 1;
        cubeIndices[i + 33] = offset + 1;
        cubeIndices[i + 34] = offset + 0;
        cubeIndices[i + 35] = offset + 4;

        offset += 8;
    }

    std::shared_ptr<IndexBuffer> cubeIB = std::make_shared<IndexBuffer>(cubeIndices, s_Data.MaxIndices);
    s_Data.CubeVertexArray->setIndexBuffer(cubeIB);
    delete[] cubeIndices;
}

Renderer::~Renderer()
{
    delete[] s_Data.CubeVertexBufferBase;
}

void Renderer::beginScene(Camera& camera)
{
    s_SceneData->ViewProjectionMatrix = camera.getProjectionMatrix();
    glViewport(0, 0, m_FBWidth, m_FBHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Shader->bind();
    m_Shader->setUniformMatrix4fv("u_ViewProjection", s_SceneData->ViewProjectionMatrix);

    startBatch();
}

void Renderer::endScene(GLFWwindow*& window)
{
    flush();
    printf("foo: %i\n", s_Data.Stats.DrawCalls);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::drawCube(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    if (s_Data.CubeIndexCount >= RendererData::MaxIndices)
        nextBatch();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), {1.0f, 0.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), {0.0f, 1.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), scale);

    auto textureID = TextureManager::loadTexture("assets/texture_02.png").id;

    float textureIndex = 0.0f; // Default white texture
    for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if (s_Data.TextureSlots[i]->id == textureID)
        {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = TextureManager::getTextureByID(textureID);
        s_Data.TextureSlotIndex++;
    }

    glm::vec4 cubeVertices[8] = {
        // Front face
        glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f), // Bottom-left
        glm::vec4(0.5f, -0.5f, 0.5f, 1.0f),  // Bottom-right
        glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),   // Top-right
        glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f),  // Top-left

        // Back face
        glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), // Bottom-left
        glm::vec4(0.5f, -0.5f, -0.5f, 1.0f),  // Bottom-right
        glm::vec4(0.5f, 0.5f, -0.5f, 1.0f),   // Top-right
        glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f)   // Top-left
    };

    glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

    for (int i = 0; i < 8; i++)
    {
        s_Data.CubeVertexBufferPtr->Position = transform * cubeVertices[i];
        s_Data.CubeVertexBufferPtr->TexCoord = textureCoords[i % 4];
        s_Data.CubeVertexBufferPtr->TexIndex = textureIndex;
        s_Data.CubeVertexBufferPtr++;
    }

    s_Data.CubeIndexCount += 36; // 36 indices for a cube
}

void Renderer::startBatch()
{
    s_Data.CubeIndexCount = 0;
    s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;
}

void Renderer::nextBatch()
{
    flush();
    startBatch();
}

void Renderer::flush()
{
    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CubeVertexBufferPtr - (uint8_t*)s_Data.CubeVertexBufferBase);
    s_Data.CubeVertexBuffer->SetData(s_Data.CubeVertexBufferBase, dataSize);

    // for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
    //     s_Data.TextureSlots[i]->bind(i);

    m_Shader->bind();
    draw(s_Data.CubeVertexArray, s_Data.CubeIndexCount);
    s_Data.Stats.DrawCalls++;
}

void Renderer::draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
{
    vertexArray->bind();
    uint32_t count = indexCount ? indexCount : vertexArray->getIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void Renderer::ResetStats()
{
    s_Data.Stats = {0};
}

Statistics Renderer::GetStats()
{
    return Renderer::s_Data.Stats;
}