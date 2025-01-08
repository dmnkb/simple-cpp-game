#include "Renderer.h"
#include "Scene.h"
#include "Shader.h"
#include "Window.h"
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

    glGenBuffers(1, &s_Data.instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.instanceBuffer);

    prepareLightingUBO();
}

// TODO:
// void Renderer::render(const Scene& scene) {
//     // Shadow pass for shadow-casting lights
//     for (const auto& light : scene.getLights()) {
//         if (light.castsShadows()) {
//             Camera shadowCamera = light.createShadowCamera();
//             setActiveCamera(&shadowCamera);
//             executeShadowPass(scene, light);
//         }
//     }

//     // Reflection pass (e.g., water surface)
//     Camera reflectionCamera = createReflectionCamera(scene.getMainCamera());
//     setActiveCamera(&reflectionCamera);
//     executeReflectionPass(scene);

//     // Main pass using the player's camera
//     setActiveCamera(scene.getMainCamera());
//     executeMainPass(scene);
// }

void Renderer::update()
{
    // ==================
    // Shadow caster pass
    // ==================

    for (const auto& lightSceneNode : Scene::getLightSceneNodes())
    {
        const auto light = lightSceneNode->prepareLight();
        if (light.lightType == SPOT)
        {
            RenderPass shadowCasterPass;
            shadowCasterPass.bind(FRAMEBUFFER);

            // Setup shadow caster camera
            // TODO: Camera shadowCamera = light.createShadowCamera();
            CameraProps shadowCamProps = {45.0f * (M_PI / 180.0f), 1, 0.1f, 1000.0f};
            const auto shadowCamera = CreateRef<Camera>(shadowCamProps);
            shadowCamera->setPosition(light.position);
            shadowCamera->lookAt(light.rotation);
            Scene::setActiveCamera(shadowCamera);

            // Render meshes
            auto opaqueQueue =
                Scene::getRenderQueue([](const Renderable& renderable) { return renderable.isOpaque(); });
            drawQueue(opaqueQueue);

            // Store depth buffers along with individual transforms
            s_Data.shadowCasters.push_back({lightSceneNode->getTransform(), shadowCasterPass.getResult()});

            // Reset and cleanup
            Scene::setActiveCamera(shadowCamera);
            shadowCasterPass.unbind();
        }
    }

    // ===================================
    // Main pass using the player's camera
    // ===================================

    RenderPass opaquePass;

    // Get the scene's main camera (default = player's camera)
    Scene::setActiveCamera(Scene::getDefaultCamera());

    // Rendertarget = FBO
    opaquePass.bind(SCREEN);
    const auto opaquePassResult = opaquePass.getResult();

    // Render all meshes (currently opaque only)
    auto opaqueQueue = Scene::getRenderQueue([](const Renderable& renderable) { return renderable.isOpaque(); });
    drawQueue(opaqueQueue);

    // Clean up FBO
    opaquePass.unbind();

    // ========================
    // Bloom pass (Pseudo Code)
    // ========================

    // RenderPass bloomPass;

    // Get the scene's main camera (default = player's camera)
    // Scene::setActiveCamera(Scene::getDefaultCamera());

    // Rendertarget = Screen
    // bloomPass.bind(SCREEN);

    // Render fullscreen quad, bind bloom shader and apply the previous pass's result as texture
    // bloomShader.bind();
    // renderFullScreenQuad(opaquePassResult);
    // bloomShader.unbind();
    // bloomPass.unbind();
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

// TODO: Make so that the shader can be overridden by albedo, position, ... shader (?)
void Renderer::drawQueue(const RenderQueue& queue)
{
    Light lightBuffer[256];

    const auto lights = Scene::getLightSceneNodes();

    for (int i = 0; i < lights.size(); i++)
        lightBuffer[i] = ((lights)[i])->prepareLight();

    for (const auto& [shader, meshMap] : queue)
    {
        shader->bind();

        assert(Scene::getActiveCamera() && "No active camera in scene");

        const auto viewMatrix = Scene::getActiveCamera()->getViewMatrix();
        const auto projectionMatrix = Scene::getActiveCamera()->getProjectionMatrix();
        const auto camPos = Scene::getActiveCamera()->getPosition();

        const auto viewProjectionMatrix = projectionMatrix * viewMatrix;

        shader->setUniformMatrix4fv("u_ViewProjection", viewProjectionMatrix);
        shader->setUniform3fv("viewPos", camPos);

        GLuint uboBindingPoint = 0;
        GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), "LightsBlock");
        assert(blockIndex != GL_INVALID_INDEX && "LightsBlock not found in shader!");

        glUniformBlockBinding(shader->getProgramID(), blockIndex, uboBindingPoint);
        glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, s_Data.uboLights);

        glBindBuffer(GL_UNIFORM_BUFFER, s_Data.uboLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightBuffer), lightBuffer);

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