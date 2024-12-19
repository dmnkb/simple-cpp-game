#include "Renderer.h"
#include "Shader.h"
#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

static RendererData s_Data;
static RenderStats s_Stats;

void Renderer::init()
{
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    enableOpenGLDebugOutput();

    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Buffer for instanced rendering
    glGenBuffers(1, &s_Data.instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.instanceBuffer);

    // Lighting
    unsigned int NUM_LIGHTS = 256;

    // Create and bind the Uniform Buffer Object
    glGenBuffers(1, &s_Data.uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, s_Data.uboLights);

    // Allocate memory for the UBO (empty for now)
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * NUM_LIGHTS, nullptr, GL_DYNAMIC_DRAW);

    // Bind the UBO to the binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, s_Data.uboLights);
}

void Renderer::beginScene(Camera& camera)
{
    s_Data.viewMatrix = camera.getViewMatrix();
    s_Data.viewProjectionMatrix = camera.getProjectionMatrix();
    s_Data.camPos = camera.getPosition();
}

void Renderer::update(const glm::vec2& windowDimensions)
{
    glViewport(0, 0, windowDimensions.x, windowDimensions.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawQueue();

    // Reset renderQueue
    s_Data.renderQueue.opaque.clear();
    s_Data.renderQueue.transparent.clear();

    // Reset list of lights
    s_Data.lights->clear();
}

void Renderer::submitRenderable(const Renderable& renderable)
{
    if (!renderable.mesh || !renderable.shader)
    {
        if (!renderable.mesh)
            std::cerr << "Invalid mesh in batch!" << std::endl;
        if (!renderable.shader)
            std::cerr << "Invalid shader in batch!" << std::endl;
        return;
    }

    s_Data.renderQueue.opaque[renderable.shader][renderable.mesh].push_back(renderable.transform);
}

void Renderer::submitLight(const Light& light)
{
    s_Data.lights->push_back(light);
}

void Renderer::bindInstanceData(const std::vector<glm::mat4>& transforms)
{
    // Bind the instance buffer and upload transforms
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.instanceBuffer);

    // Upload instance transforms to the buffer
    glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_DYNAMIC_DRAW);

    // Set up vertex attribute pointers for instance transforms
    // Enable attribute pointers for the mat4
    GLsizei vec4Size = sizeof(glm::vec4);
    for (int i = 0; i < 4; ++i)
    {
        glEnableVertexAttribArray(3 + i); // 3, 4, 5, 6 are the locations
        // glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
        //                       reinterpret_cast<void*>(static_cast<std::uintptr_t>(i * vec4Size)));
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor(3 + i, 1); // One mat4 per instance
    }
}

void Renderer::unbindInstancBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::drawBatch(const std::shared_ptr<Mesh>& mesh, const std::vector<glm::mat4>& transforms)
{
    s_Stats.drawCallCount++;

    mesh->bind();
    bindInstanceData(transforms);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr, transforms.size());
    unbindInstancBuffer();
    mesh->unbind();
}

void Renderer::drawQueue()
{
    Light lightBuffer[256];
    for (int i = 0; i < s_Data.lights->size(); i++)
        lightBuffer[i] = (*s_Data.lights)[i];

    for (const auto& [shader, meshMap] : s_Data.renderQueue.opaque)
    {
        shader->bind();

        glm::mat4 viewProjectionMatrix = s_Data.viewProjectionMatrix * s_Data.viewMatrix;

        shader->setUniformMatrix4fv("u_ViewProjection", viewProjectionMatrix);
        shader->setUniform3fv("viewPos", s_Data.camPos);

        GLuint uboBindingPoint = 0;
        GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), "LightsBlock");
        assert(blockIndex != GL_INVALID_INDEX && "LightsBlock not found in shader!");

        glUniformBlockBinding(shader->getProgramID(), blockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, s_Data.uboLights);

        glBindBuffer(GL_UNIFORM_BUFFER, s_Data.uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightBuffer), lightBuffer);

        for (const auto& [mesh, transforms] : meshMap)
        {
            drawBatch(mesh, transforms);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        shader->unbind();
    }
}

const RenderStats& Renderer::getStats()
{
    return s_Stats;
}

const void Renderer::resetStats()
{
    s_Stats.drawCallCount = 0;
}

void GLAPIENTRY Renderer::debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                        const GLchar* message, const void* userParam)
{
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        std::cerr << "High Severity: " << message << std::endl;
    }
}

void Renderer::enableOpenGLDebugOutput()
{

    GLint majorVersion, minorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

    // glDebugMessageCallback is only supported after OpoenGL V.4.3 (So not on MacOS)
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 3))
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        std::cout << "OpenGL Debug Output Enabled" << std::endl;
    }
    else
    {
        std::cout << "OpenGL Debug Output not supported for this context." << std::endl;
    }
}