#include "Renderer.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

static const GLfloat vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 0
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // B 1
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // C 2
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // D 3

    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // E 4
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // F 5
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // G 6
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // H 7

    // Left face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 8
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f,  // D 9
    -0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // H 10
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,  // E 11

    // Right face
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // B 12
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f,  // C 13
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // G 14
    0.5f, -0.5f, 0.5f, 0.0f, 1.0f,  // F 15

    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // A 16
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // B 17
    0.5f, -0.5f, 0.5f, 1.0f, 1.0f,   // F 18
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,  // E 19

    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, // D 20
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f,  // C 21
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // G 22
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f   // H 23
};

static const GLfloat normals[] = {
    // Back face normals
    0.0f, 0.0f, -1.0f, // A 0
    0.0f, 0.0f, -1.0f, // B 1
    0.0f, 0.0f, -1.0f, // C 2
    0.0f, 0.0f, -1.0f, // D 3

    // Front face normals
    0.0f, 0.0f, 1.0f, // E 4
    0.0f, 0.0f, 1.0f, // F 5
    0.0f, 0.0f, 1.0f, // G 6
    0.0f, 0.0f, 1.0f, // H 7

    // Left face normals
    -1.0f, 0.0f, 0.0f, // A 8
    -1.0f, 0.0f, 0.0f, // D 9
    -1.0f, 0.0f, 0.0f, // H 10
    -1.0f, 0.0f, 0.0f, // E 11

    // Right face normals
    1.0f, 0.0f, 0.0f, // B 12
    1.0f, 0.0f, 0.0f, // C 13
    1.0f, 0.0f, 0.0f, // G 14
    1.0f, 0.0f, 0.0f, // F 15

    // Bottom face normals
    0.0f, -1.0f, 0.0f, // A 16
    0.0f, -1.0f, 0.0f, // B 17
    0.0f, -1.0f, 0.0f, // F 18
    0.0f, -1.0f, 0.0f, // E 19

    // Top face normals
    0.0f, 1.0f, 0.0f, // D 20
    0.0f, 1.0f, 0.0f, // C 21
    0.0f, 1.0f, 0.0f, // G 22
    0.0f, 1.0f, 0.0f, // H 23
};

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 UV;
    glm::vec3 Normal;
    glm::float32 textureID;
};

struct RendererData
{
    static const uint32_t maxCubes = 10000;
    static const uint32_t maxIndices = maxCubes * 36;
    static const uint32_t maxVertices = maxCubes * 24;

    GLuint vertexArray, vertexBuffer, indexBuffer;
    uint32_t indices[maxIndices];

    glm::mat4 viewProjectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 camPos;
    // TODO: unique_ptr?
    std::shared_ptr<Shader> shader;
    // TODO: Get texture slot count by driver?
    GLint textureIDs[16];

    uint32_t indexCount, vertexCount = 0;
    Vertex* vertexBufferBase = nullptr;
    Vertex* vertexBufferPtr = nullptr;
};

static RendererData s_Data;

Renderer::Statistics stats;

void Renderer::init()
{
    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glGenVertexArrays(1, &s_Data.vertexArray);
    glBindVertexArray(s_Data.vertexArray);

    glGenBuffers(1, &s_Data.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * RendererData::maxVertices, nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &s_Data.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.indexBuffer);

    uint32_t indices[RendererData::maxIndices];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < RendererData::maxIndices; i += 36)
    {
        // Back face
        indices[i + 0] = offset + 2;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 0;
        indices[i + 3] = offset + 0;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 2;

        // Front face
        indices[i + 6] = offset + 4;
        indices[i + 7] = offset + 5;
        indices[i + 8] = offset + 6;
        indices[i + 9] = offset + 6;
        indices[i + 10] = offset + 7;
        indices[i + 11] = offset + 4;

        // Left face
        indices[i + 12] = offset + 10;
        indices[i + 13] = offset + 9;
        indices[i + 14] = offset + 8;
        indices[i + 15] = offset + 8;
        indices[i + 16] = offset + 11;
        indices[i + 17] = offset + 10;

        // Right face
        indices[i + 18] = offset + 12;
        indices[i + 19] = offset + 13;
        indices[i + 20] = offset + 14;
        indices[i + 21] = offset + 14;
        indices[i + 22] = offset + 15;
        indices[i + 23] = offset + 12;

        // Bottom face
        indices[i + 24] = offset + 16;
        indices[i + 25] = offset + 17;
        indices[i + 26] = offset + 18;
        indices[i + 27] = offset + 18;
        indices[i + 28] = offset + 19;
        indices[i + 29] = offset + 16;

        // Top face
        indices[i + 30] = offset + 22;
        indices[i + 31] = offset + 21;
        indices[i + 32] = offset + 20;
        indices[i + 33] = offset + 20;
        indices[i + 34] = offset + 23;
        indices[i + 35] = offset + 22;

        offset += 24; // 24 unique vertices for a cube
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, UV));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureID));
    glEnableVertexAttribArray(3);

    s_Data.vertexBufferBase = new Vertex[RendererData::maxVertices];
    if (s_Data.vertexBufferBase == nullptr)
    {
        std::cerr << "Failed to allocate memory for vertex buffer base" << std::endl;
        exit(EXIT_FAILURE);
    }
    s_Data.vertexBufferPtr = s_Data.vertexBufferBase;

    s_Data.shader = std::make_shared<Shader>(vertex_shader_text, fragment_shader_text);
}

void Renderer::shutdown()
{
    delete[] s_Data.vertexBufferBase;
}

void Renderer::beginScene(Camera& camera)
{
    s_Data.viewMatrix = camera.getViewMatrix();
    s_Data.camPos = camera.getPosition();
    s_Data.viewProjectionMatrix = camera.getProjectionMatrix();

    startBatch();
}

void Renderer::endScene(GLFWwindow*& window)
{
    flush();
}

void Renderer::submitCube(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::float32 textureID)
{
    if (s_Data.indexCount >= RendererData::maxIndices)
        nextBatch();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), {1.0f, 0.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), {0.0f, 1.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), scale);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

    for (int i = 0; i < 24; i++)
    {
        if (i * 5 + 4 >= sizeof(vertices) / sizeof(vertices[0])) // Ensure we are within bounds
        {
            std::cerr << "Index out of bounds: " << i * 5 + 4 << std::endl;
            exit(EXIT_FAILURE);
        }

        glm::vec3 vertex(vertices[i * 5], vertices[i * 5 + 1], vertices[i * 5 + 2]);
        glm::vec2 uv(vertices[i * 5 + 3], vertices[i * 5 + 4]);
        glm::vec3 normal(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);

        glm::vec4 transformedVertex = transform * glm::vec4(vertex, 1.0f);
        glm::vec3 transformedNormal = normalMatrix * normal;

        s_Data.vertexBufferPtr->Position = glm::vec3(transformedVertex);
        s_Data.vertexBufferPtr->UV = uv;
        s_Data.vertexBufferPtr->Normal = transformedNormal;
        s_Data.vertexBufferPtr->textureID = textureID;

        // FIXME: OpenGL textureIDs might not count like
        // 0, 1, 2, ... and might therefore not be good
        // for indexing
        s_Data.textureIDs[static_cast<int>(textureID)] = textureID;

        glActiveTexture(textureID + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        s_Data.vertexBufferPtr++;

        if (s_Data.vertexBufferPtr - s_Data.vertexBufferBase > RendererData::maxVertices) // Ensure we are within bounds
        {
            std::cerr << "Vertex buffer pointer out of bounds" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    s_Data.indexCount += 36;
    s_Data.vertexCount += 24;
    stats.vertexCount += 24;
    stats.cubeCount++;
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
    if (dataSize > RendererData::maxVertices * sizeof(Vertex)) // Ensure we are within bounds
    {
        std::cerr << "Data size out of bounds: " << dataSize << std::endl;
        exit(EXIT_FAILURE);
    }

    // Upload vertex buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_Data.vertexBufferBase);

    // Bind shader and set uniforms
    s_Data.shader->bind();
    glm::mat4 viewProjection = s_Data.viewProjectionMatrix * s_Data.viewMatrix;

    s_Data.shader->setUniformMatrix4fv("u_ViewProjection", viewProjection);
    s_Data.shader->setUniform3fv("viewPos", s_Data.camPos);
    s_Data.shader->setUniform3fv("lightPos", glm::vec3(20, 20, 10));
    s_Data.shader->setUniform1iv("u_Textures", s_Data.textureIDs);

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
    stats.vertexCount = 0;
    stats.cubeCount = 0;
}

Renderer::Statistics Renderer::getStats()
{
    return stats;
}

glm::mat4 Renderer::getVPM()
{
    return s_Data.viewProjectionMatrix;
}
