#include "Renderer.h"
#include "Shader.h"
#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

/**
 * transparent
 * L    Material
 *      L   Mesh (sorted from far to near)
 *          L vector<glm::mat4>
 * opaque
 * L    Material
 *      L   Mesh (sorted from far to near)
 *          L vector<glm::mat4>
 */
struct RenderQueue
{
    // clang-format off
    std::unordered_map<std::shared_ptr<Shader>, 
        std::unordered_map<std::shared_ptr<Mesh>, 
            std::vector<glm::mat4>>> transparent;
            
    std::unordered_map<std::shared_ptr<Shader>, 
        std::unordered_map<std::shared_ptr<Mesh>, 
            std::vector<glm::mat4>>> opaque;
    // clang-format on
};

struct RendererData
{
    glm::mat4 viewProjectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 camPos;

    GLuint instanceBuffer;
    Light lightArray[256];
    GLuint uboLights;

    RenderQueue renderQueue;
};

static RendererData s_Data;

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

void Renderer::update()
{
    draw();

    // Clear the mesh batches after drawing
    s_Data.renderQueue.opaque.clear();
    s_Data.renderQueue.transparent.clear();
}

void Renderer::submitRenderable(Renderable renderable)
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

void Renderer::submitLights(const std::vector<Light>& lights)
{
    const unsigned int lightCount = lights.size();

    for (int i = 0; i < lightCount; i++)
        s_Data.lightArray[i] = lights[i];
}

void Renderer::draw()
{
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
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(s_Data.lightArray), s_Data.lightArray);

        for (const auto& [mesh, transforms] : meshMap)
        {
            mesh->bind(s_Data.instanceBuffer);
            for (const auto& transform : transforms)
            {
                shader->setUniformMatrix4fv("u_Transform", transform);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            mesh->unbind();
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        shader->unbind();
    }
}

#define PRINT_ERRORS 1

void Renderer::drawInstanced()
{

    //     GLenum error = glGetError();
    //     for (const auto& [mesh, transforms] : s_Data.renderQueue)
    //     {
    //         if (!mesh)
    //         {
    //             std::cerr << "Invalid mesh in batch!" << std::endl;
    //         }
    // #if PRINT_ERRORS
    //         std::cout << "Mesh batch size: " << transforms.size() << std::endl;
    // #endif

    //         // Ensure mesh is valid
    //         if (!mesh || transforms.empty())
    //             continue;

    //         // The number of instances to render is the size of the transforms vector
    //         size_t instanceCount = transforms.size();

    //         // Bind the mesh (assumes mesh->bind() sets up VAO, VBOs, etc.)
    //         mesh->bind(s_Data.instanceBuffer);

    // #if PRINT_ERRORS
    //         if (error != GL_NO_ERROR)
    //         {
    //             std::cerr << "OpenGL error (1): " << error << std::endl;
    //             error = GL_NO_ERROR;
    //         }
    // #endif

    //         // Upload instance transforms to the GPU (assumes a buffer is prepared for this)
    //         // Example: bind and update an instance buffer
    //         glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_DYNAMIC_DRAW);

    // #if PRINT_ERRORS
    //         if (error != GL_NO_ERROR)
    //         {
    //             std::cerr << "OpenGL error (2): " << error << std::endl;
    //             error = GL_NO_ERROR;
    //         }
    // #endif

    //         // Ensure the instance buffer is associated with a vertex attribute
    //         // Assuming location 3 is for instance transforms (mat4 uses 4 locations)
    //         for (int i = 0; i < 4; ++i)
    //         {
    //             glEnableVertexAttribArray(3 + i);
    //             glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) *
    //             i)); glVertexAttribDivisor(3 + i, 1); // One instance per transform
    //         }

    // #if PRINT_ERRORS
    //         if (error != GL_NO_ERROR)
    //         {
    //             std::cerr << "OpenGL error (3): " << error << std::endl;
    //             error = GL_NO_ERROR;
    //         }
    // #endif

    //         // Draw the mesh instances
    //         glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr, instanceCount);

    // #if PRINT_ERRORS
    //         error = glGetError();
    //         if (error != GL_NO_ERROR)
    //         {
    //             std::cerr << "OpenGL error (4): " << error << std::endl;
    //         }
    // #endif

    //         // Unbind mesh and buffers
    //         glBindBuffer(GL_ARRAY_BUFFER, 0);
    //         mesh->unbind();
    //     }
}

// Debug callback function for OpenGL errors
void GLAPIENTRY Renderer::debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                        const GLchar* message, const void* userParam)
{
    // Print the error information
    std::cerr << "OpenGL Debug Message: " << message << std::endl;

    // You can filter the severity if you want to handle it more specifically
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        std::cerr << "High Severity: " << message << std::endl;
    }
}

void Renderer::enableOpenGLDebugOutput()
{
    // Only enable if OpenGL version supports debugging (4.3+)
    GLint majorVersion, minorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

    // Enable debugging if the OpenGL version is 4.3 or greater
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