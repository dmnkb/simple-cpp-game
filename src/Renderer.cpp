#include "Renderer.h"
#include "Shader.h"
#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

struct RendererData
{
    glm::mat4 viewProjectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 camPos;

    // TODO: Consider a pair of Mesh and Material as hash. Transforms should be sorted from far to near
    // A Renderable pool might be more meory efficiant
    std::unordered_map<std::shared_ptr<Mesh>, std::vector<glm::mat4>> meshBatches;

    // TODO: remove later
    std::unique_ptr<Shader> shader;
    GLuint instanceBuffer;
    GLuint uboLights;
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

    // FIXME: Temporary shader
    s_Data.shader = std::make_unique<Shader>("assets/phong.vs", "assets/phong.fs");

    // Lighting
    unsigned int NUM_LIGHTS = 256;

    // Link the UBO to binding point 0 (match shader layout)
    // TODO: Cache the block index since the shaders are only available during the update loop
    // GLuint uboBindingPoint = 0;
    // GLuint blockIndex = glGetUniformBlockIndex(s_Data.shader->getProgramID(), "LightsBlock");
    // assert(blockIndex != GL_INVALID_INDEX && "LightsBlock not found in shader!");

    // glUniformBlockBinding(s_Data.shader->getProgramID(), blockIndex, uboBindingPoint);

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
}

void Renderer::update()
{
    // FIXME: bind shader per batch
    s_Data.shader->bind();

    glm::mat4 viewProjection = s_Data.viewProjectionMatrix * s_Data.viewMatrix;

    s_Data.shader->setUniformMatrix4fv("u_ViewProjection", viewProjection);
    s_Data.shader->setUniform3fv("viewPos", s_Data.camPos);

    // GLint activeAttributes;
    // glGetProgramiv(s_Data.shader->getProgramID(), GL_ACTIVE_ATTRIBUTES, &activeAttributes);
    // std::cout << "Active Attributes: " << activeAttributes << std::endl;

    drawInstanced();
    s_Data.shader->unbind();

    // Clear the mesh batches after drawing
    s_Data.meshBatches.clear();
}

void Renderer::submitRenderable(Renderable renderable)
{
    // TODO: Assert data completeness
    s_Data.meshBatches[renderable.mesh].push_back(renderable.transform);
}

void Renderer::submitLights(const std::vector<Light>& lights)
{
    const unsigned int lightCount = lights.size();
    Light lightArray[lightCount];

    // Copy data from the vector to the array
    for (int i = 0; i < lightCount; i++)
        lightArray[i] = lights[i];

    glBindBuffer(GL_UNIFORM_BUFFER, s_Data.uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightArray), lightArray);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

#define PRINT_ERRORS 1

void Renderer::drawInstanced()
{

    GLenum error = glGetError();
    for (const auto& [mesh, transforms] : s_Data.meshBatches)
    {
        if (!mesh)
        {
            std::cerr << "Invalid mesh in batch!" << std::endl;
        }
#if PRINT_ERRORS
        std::cout << "Mesh batch size: " << transforms.size() << std::endl;
#endif

        // Ensure mesh is valid
        if (!mesh || transforms.empty())
            continue;

        // The number of instances to render is the size of the transforms vector
        size_t instanceCount = transforms.size();

        // Bind the mesh (assumes mesh->bind() sets up VAO, VBOs, etc.)
        mesh->bind(s_Data.instanceBuffer);

#if PRINT_ERRORS
        if (error != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error (1): " << error << std::endl;
            error = GL_NO_ERROR;
        }
#endif

        // Upload instance transforms to the GPU (assumes a buffer is prepared for this)
        // Example: bind and update an instance buffer
        glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_DYNAMIC_DRAW);

#if PRINT_ERRORS
        if (error != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error (2): " << error << std::endl;
            error = GL_NO_ERROR;
        }
#endif

        // Ensure the instance buffer is associated with a vertex attribute
        // Assuming location 3 is for instance transforms (mat4 uses 4 locations)
        for (int i = 0; i < 4; ++i)
        {
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
            glVertexAttribDivisor(3 + i, 1); // One instance per transform
        }

#if PRINT_ERRORS
        if (error != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error (3): " << error << std::endl;
            error = GL_NO_ERROR;
        }
#endif

        // Draw the mesh instances
        glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr, instanceCount);

#if PRINT_ERRORS
        error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error (4): " << error << std::endl;
        }
#endif

        // Unbind mesh and buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        mesh->unbind();
    }
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