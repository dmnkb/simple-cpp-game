#include "Renderer.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

struct Vertex
{
    glm::vec3 Position;
};

struct RendererData
{
    static const uint32_t maxCubes = 2; // for debugging purposes
    static const uint32_t maxIndices = maxCubes * 36;
    static const uint32_t maxVertices = maxCubes * 8;

    GLuint vertexArray, vertexBuffer, indexBuffer;

    glm::mat4 viewProjectionMatrix;
    glm::mat4 viewMatrix;
    std::shared_ptr<Shader> shader;

    uint32_t indexCount, vertexCount = 0;
    Vertex* vertexBufferBase = nullptr;
    Vertex* vertexBufferPtr = nullptr;
};

static RendererData s_Data;

Renderer::Statistics stats;

static const GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 0
    0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, // B 1
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, // C 2
    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, // D 3
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, // E 4
    0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, // F 5
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // G 6
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f  // H 7
};

void Renderer::init()
{
    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &s_Data.vertexArray);
    glBindVertexArray(s_Data.vertexArray);

    glGenBuffers(1, &s_Data.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * RendererData::maxVertices, nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &s_Data.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.indexBuffer);

    uint32_t indices[RendererData::maxIndices];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < RendererData::maxIndices; i += 36)
    {
        // Back face
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 3;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 1;
        indices[i + 5] = offset + 0;

        // Front face
        indices[i + 6] = offset + 4;
        indices[i + 7] = offset + 5;
        indices[i + 8] = offset + 6;
        indices[i + 9] = offset + 6;
        indices[i + 10] = offset + 7;
        indices[i + 11] = offset + 4;

        // Left face
        indices[i + 12] = offset + 0;
        indices[i + 13] = offset + 3;
        indices[i + 14] = offset + 7;
        indices[i + 15] = offset + 7;
        indices[i + 16] = offset + 4;
        indices[i + 17] = offset + 0;

        // Right face
        indices[i + 18] = offset + 1;
        indices[i + 19] = offset + 5;
        indices[i + 20] = offset + 6;
        indices[i + 21] = offset + 6;
        indices[i + 22] = offset + 2;
        indices[i + 23] = offset + 1;

        // Bottom face
        indices[i + 24] = offset + 0;
        indices[i + 25] = offset + 1;
        indices[i + 26] = offset + 5;
        indices[i + 27] = offset + 5;
        indices[i + 28] = offset + 4;
        indices[i + 29] = offset + 0;

        // Top face
        indices[i + 30] = offset + 3;
        indices[i + 31] = offset + 2;
        indices[i + 32] = offset + 6;
        indices[i + 33] = offset + 6;
        indices[i + 34] = offset + 7;
        indices[i + 35] = offset + 3;

        offset += 8; // 8 unique vertices for a cube
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    const size_t POSITION_SIZE = 3;
    const size_t STRIDE = POSITION_SIZE * sizeof(GLfloat);

    // Position attribute
    glVertexAttribPointer(0, POSITION_SIZE, GL_FLOAT, GL_FALSE, STRIDE, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    s_Data.vertexBufferBase = new Vertex[RendererData::maxVertices];
    s_Data.shader = std::make_shared<Shader>(vertex_shader_text, fragment_shader_text);

    // Bind texture and set uniform
    auto textureID = TextureManager::loadTexture("assets/texture_02.png").id;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    s_Data.shader->setUniform1i("myTextureSampler", 0);
}

void Renderer::shutdown()
{
    delete[] s_Data.vertexBufferBase;
}

void Renderer::beginScene(Camera& camera)
{
    s_Data.viewMatrix = camera.getViewMatrix();
    s_Data.viewProjectionMatrix = camera.getProjectionMatrix();

    startBatch();
}

void Renderer::endScene(GLFWwindow*& window)
{
    flush();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Renderer::drawCube(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    if (s_Data.indexCount >= RendererData::maxIndices)
        nextBatch();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), {1.0f, 0.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), {0.0f, 1.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), scale);

    // Loop through the static vertices array and transform each vertex
    for (int i = 0; i < 8; i++)
    {
        glm::vec3 vertex(vertices[i * 5], vertices[i * 5 + 1], vertices[i * 5 + 2]); // Extract vertex position
        glm::vec4 transformedVertex = transform * glm::vec4(vertex, 1.0f);
        s_Data.vertexBufferPtr->Position = glm::vec3(transformedVertex);
        s_Data.vertexBufferPtr++;
    }

    s_Data.indexCount += 36;
    s_Data.vertexCount += 8;
}

void Renderer::startBatch()
{
    s_Data.indexCount = 0;
    s_Data.vertexBufferPtr = s_Data.vertexBufferBase;
}

void Renderer::nextBatch()
{
    flush();
    startBatch();
}

void Renderer::flush()
{
    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.vertexBufferPtr - (uint8_t*)s_Data.vertexBufferBase);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_Data.vertexBufferBase);

    s_Data.shader->bind();
    glm::mat4 viewProjection = s_Data.viewProjectionMatrix * s_Data.viewMatrix;
    s_Data.shader->setUniformMatrix4fv("u_ViewProjection", viewProjection);

    draw();

    stats.drawCalls++;
}

void Renderer::draw()
{
    glBindVertexArray(s_Data.vertexArray);
    glDrawElements(GL_TRIANGLES, s_Data.indexCount, GL_UNSIGNED_INT, 0);
}

void Renderer::resetStats()
{
    stats.drawCalls = 0;
}

Renderer::Statistics Renderer::getStats()
{
    return stats;
}

glm::mat4 Renderer::getVPM()
{
    return s_Data.viewProjectionMatrix;
}
