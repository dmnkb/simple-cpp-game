#include "Renderer.h"
#include "Scene.h"
#include "Shader.h"
#include "Window.h"
#include "pch.h"
#include <fmt/core.h>
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
    // glDepthMask(GL_TRUE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glGenBuffers(1, &s_Data.instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.instanceBuffer);

    prepareLightingUBO();
}

void Renderer::update()
{
    // ===============
    // Shadow map pass
    // ===============

    // Clean up list of shadow casters and free memory
    for (auto& [_, texture] : s_Data.shadowCasters)
    {
        texture->unbind();
        glDeleteTextures(1, &texture->id);
        texture.reset();
    }
    s_Data.shadowCasters.clear();

    static auto depthShader = CreateRef<Shader>("assets/depth.vs", "assets/depth.fs");
    depthShader->bind();

    for (auto& lightSceneNode : Scene::getLightSceneNodes())
    {
        // auto depthTexture = TextureManager::createColorTexture(Window::getFrameBufferDimensions());
        auto depthTexture = TextureManager::createDepthTexture(Window::getFrameBufferDimensions());
        auto shadowMapPassQueue =
            Scene::getRenderQueue([](const Ref<MeshSceneNode>& node) { return node->isOpaque(); });

        static RenderPass shadowCasterPass;
        shadowCasterPass.bind(depthTexture);

        auto shadowCam = lightSceneNode->createShadowCamera();
        Scene::setActiveCamera(shadowCam);

        const auto lightSpaceMatrix = shadowCam->getProjectionMatrix() * shadowCam->getViewMatrix();
        depthShader->setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

        glCullFace(GL_FRONT);
        executeShadowPass(shadowMapPassQueue);
        glCullFace(GL_BACK);

        s_Data.shadowCasters.push_back({lightSpaceMatrix, depthTexture});

        Scene::clearRenderQueue();
        shadowCasterPass.unbind();
    }

    depthShader->unbind();

    // ===================================
    // Main pass using the player's camera
    // ===================================
    Scene::setActiveCamera(Scene::getDefaultCamera());

    static RenderPass opaquePass;
    opaquePass.bind();

    auto finalQueue = Scene::getRenderQueue([](const Ref<MeshSceneNode>& node) { return node->isOpaque(); });
    executePass(finalQueue);
    Scene::clearRenderQueue();

    opaquePass.unbind();
}

void Renderer::executeShadowPass(const RenderQueue& queue)
{
    for (const auto& [_, meshMap] : queue)
    {
        for (const auto& [mesh, transforms] : meshMap)
        {
            drawInstanceBatch(mesh, transforms);
        }
    }
}

void Renderer::executePass(const RenderQueue& queue)
{

    Light lightBuffer[256];

    const auto lights = Scene::getLightSceneNodes();

    for (int i = 0; i < lights.size(); i++)
        lightBuffer[i] = ((lights)[i])->prepareLight();

    for (const auto& [shader, meshMap] : queue)
    {
        const auto viewMatrix = Scene::getActiveCamera()->getViewMatrix();
        const auto projectionMatrix = Scene::getActiveCamera()->getProjectionMatrix();
        const auto viewProjectionMatrix = projectionMatrix * viewMatrix;
        const auto camPos = Scene::getActiveCamera()->getPosition();

        shader->bind();
        shader->setUniformMatrix4fv("u_ViewProjection", viewProjectionMatrix);

        // View
        shader->setUniform3fv("viewPos", camPos);

        // Lighting
        GLuint uboBindingPoint = 0;
        GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), "LightsBlock");
        assert(blockIndex != GL_INVALID_INDEX && "LightsBlock not found in shader!");

        glUniformBlockBinding(shader->getProgramID(), blockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, s_Data.uboLights);

        glBindBuffer(GL_UNIFORM_BUFFER, s_Data.uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightBuffer), lightBuffer);

        for (size_t i = 0; i < s_Data.shadowCasters.size(); ++i)
        {
            auto shadowCaster = s_Data.shadowCasters[i];

            std::string uniformName = fmt::format("lightSpaceMatrices[{}]", i);
            shader->setUniformMatrix4fv(uniformName.c_str(), shadowCaster.lightSpaceMatrix);

            uniformName = fmt::format("shadowMaps[{}]", i);
            shadowCaster.depthTexture->bind(i);           // Bind texture to the i-th texture unit
            shader->setUniform1i(uniformName.c_str(), i); // Tell shader which texture unit to use
        }

        for (const auto& [mesh, transforms] : meshMap)
        {
            drawInstanceBatch(mesh, transforms);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shader->unbind();
    }
}

void Renderer::drawInstanceBatch(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms)
{
    s_Stats.drawCallCount++;

    mesh->bind();
    bindInstanceBuffer(transforms);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr, transforms.size());
    unbindInstancBuffer();
    mesh->unbind();
}

void Renderer::bindInstanceBuffer(const std::vector<glm::mat4>& transforms)
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

void Renderer::unbindInstancBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::prepareLightingUBO()
{
    unsigned int NUM_LIGHTS = 256;

    // Create and bind the Uniform Buffer Object
    glGenBuffers(1, &s_Data.uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, s_Data.uboLights);

    // Allocate memory for the UBO (empty for now)
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * NUM_LIGHTS, nullptr, GL_DYNAMIC_DRAW);

    // Bind the UBO to the binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, s_Data.uboLights);
}

const RenderStats& Renderer::getStats()
{
    return s_Stats;
}

const void Renderer::resetStats()
{
    s_Stats.drawCallCount = 0;
}

const std::vector<Ref<Texture>> Renderer::getShadowCasterDepthBuffers()
{
    std::vector<Ref<Texture>> textures = {};

    for (const auto& shadowCaster : s_Data.shadowCasters)
    {
        textures.push_back(shadowCaster.depthTexture);
    }

    return textures;
}

void Renderer::shutdown()
{
    glDeleteBuffers(1, &s_Data.instanceBuffer);
    glDeleteBuffers(1, &s_Data.uboLights);
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