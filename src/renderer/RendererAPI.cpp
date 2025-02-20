#include "RendererAPI.h"

static RendererAPIData s_Data;

void RendererAPI::init()
{
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    enableOpenGLDebugOutput();

    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // TODO:
    // Requires back-to-from sorting of transparent objects
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenBuffers(1, &s_Data.instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.instanceBuffer);
}

void RendererAPI::shutdown()
{
    glDeleteBuffers(1, &s_Data.instanceBuffer);
}

void RendererAPI::drawInstanced(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms)
{
    // TODO: Draw call stats?
    mesh->bind();
    bindInstanceBuffer(transforms);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr, transforms.size());
    unbindInstanceBuffer();
    mesh->unbind();
}

void RendererAPI::bindInstanceBuffer(const std::vector<glm::mat4>& transforms)
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
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              reinterpret_cast<void*>(static_cast<std::uintptr_t>(i * vec4Size)));
        glVertexAttribDivisor(3 + i, 1); // One mat4 per instance
    }
}

void RendererAPI::unbindInstanceBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLAPIENTRY RendererAPI::debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                           const GLchar* message, const void* userParam)
{
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        std::cerr << "High Severity: " << message << std::endl;
    }
}

void RendererAPI::enableOpenGLDebugOutput()
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