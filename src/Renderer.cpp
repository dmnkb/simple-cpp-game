#include "Renderer.h"
#include "Shader.h"
#include "Window.h"
#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

static RendererData s_Data;
static RenderStats s_Stats;

void Renderer::init(const Ref<Camera>& camera)
{
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    enableOpenGLDebugOutput();

    glClearColor(0.2902f, 0.4196f, 0.9647f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    prepareInstanceBuffer();
    prepareLighting();

    s_Data.defaultCamera = camera;

    RenderPass shadowMapPass = RenderPass(false);
    s_Data.renderPasses.push_back({shadowMapPass, ERenderQueueFilter::ALL, nullptr, {}, "shadowMapPass"});

    RenderPass simpleBloomPass = RenderPass(false);
    s_Data.renderPasses.push_back({simpleBloomPass, ERenderQueueFilter::QUAD, nullptr, {}, "simpleBloomPass"});

    RenderPass finalDrawPass = RenderPass();
    s_Data.renderPasses.push_back({finalDrawPass, ERenderQueueFilter::ALL, nullptr, {}, "finalDrawPass"});
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

// TODO:
// void Renderer::executePass(const RenderPass& pass, const std::vector<SceneObject>& objects) {
//     pass.setup();

//     for (const auto& object : objects) {
//         if (pass.filter(object)) {
//             object.draw();
//         }
//     }

//     pass.teardown();
// }

void Renderer::beginScene(const Ref<Camera>& camera)
{
    s_Data.defaultCamera = camera;
}

void Renderer::update()
{
    // Loop through all render passes and render to the defined target.
    // Then, draw all render queues within the applied filter.
    for (auto& [pass, filter, camera, results, name] : s_Data.renderPasses)
    {
        std::cout << "Begin pass: " << name << std::endl;
        if (camera)
        {
            // Set the active camera to be this camera, so that the scene can be rendered from that angle
            s_Data.activeCamera = camera;
            for (const auto light : s_Data.lights)
            {
                // Make the render passe's camera match the light transform
                updateCameraFromLight(camera, light);
                pass.setup();
                drawAll(filter);
                results.push_back(pass.getResult());
            }
        }
        else
        {
            // Reset camera to the default camera, that is controlled by the player
            s_Data.activeCamera = s_Data.defaultCamera;
            pass.setup();
            drawAll(filter);
            results.push_back(pass.getResult());
        }
    }

    // Clear all intermediate frame buffer object textures
    for (auto& [pass, filter, camera, results, name] : s_Data.renderPasses)
        results.clear();

    // Reset renderQueue
    s_Data.renderQueue.opaque.clear();
    s_Data.renderQueue.transparent.clear();

    // Reset list of lights
    s_Data.lights.clear();
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
    s_Data.lights.push_back(light);
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

void Renderer::prepareLighting()
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

void Renderer::updateCameraFromLight(Ref<Camera>& camera, const Light& light)
{
    switch (light.lightType)
    {
    case ELightType::POINT:
        // Point lights don't support shadows for now
        break;
    case ELightType::SPOT:
        camera->setPosition(light.position);
        camera->lookAt(light.rotation);
        // TODO: adjust FOV according to the cone
        break;
    default:
        break;
    }
}

void Renderer::drawAll(const ERenderQueueFilter& filter, const Ref<Shader> overrideShader)
{
    switch (filter)
    {
    case ERenderQueueFilter::ALL:
        drawQueue(s_Data.renderQueue.opaque);
        drawQueue(s_Data.renderQueue.transparent);
        break;
    case ERenderQueueFilter::OPAQUE:
        drawQueue(s_Data.renderQueue.opaque);
        break;
    case ERenderQueueFilter::TRANSPARENT:
        drawQueue(s_Data.renderQueue.transparent);
        break;
    case ERenderQueueFilter::QUAD:
        if (overrideShader)
            overrideShader->bind();
        drawFullScreenQuad();
        if (overrideShader)
            overrideShader->unbind();
        break;
    default:
        drawQueue(s_Data.renderQueue.opaque);
        drawQueue(s_Data.renderQueue.transparent);
    }
}

// TODO: Make so that the shader can be overridden by albedo, position, ... shader (?)
void Renderer::drawQueue(const RenderQueue& queue)
{
    Light lightBuffer[256];
    for (int i = 0; i < s_Data.lights.size(); i++)
        lightBuffer[i] = (s_Data.lights)[i];

    for (const auto& [shader, meshMap] : queue)
    {
        shader->bind();

        const auto viewMatrix = s_Data.activeCamera->getViewMatrix();
        const auto projectionMatrix = s_Data.activeCamera->getProjectionMatrix();
        const auto camPos = s_Data.activeCamera->getPosition();

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
            drawBatch(mesh, transforms);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        shader->unbind();
    }
}

void Renderer::drawBatch(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms)
{
    s_Stats.drawCallCount++;

    mesh->bind();
    bindInstanceData(transforms);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr, transforms.size());
    unbindInstancBuffer();
    mesh->unbind();
}

void Renderer::drawFullScreenQuad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO = 0;

    // If the VAO has not been created, generate and set it up
    if (quadVAO == 0)
    {
        // clang-format off
        float quadVertices[] = {
            // Positions     // TexCoords
            -1.0f,  1.0f,  0.0f,  1.0f,
            -1.0f, -1.0f,  0.0f,  0.0f,
             1.0f, -1.0f,  1.0f,  0.0f,

            -1.0f,  1.0f,  0.0f,  1.0f,
             1.0f, -1.0f,  1.0f,  0.0f,
             1.0f,  1.0f,  1.0f,  1.0f
        };
        // clang-format on

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    // Bind and draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind VAO to avoid accidental modification
    glBindVertexArray(0);
}

void Renderer::prepareInstanceBuffer()
{
    glGenBuffers(1, &s_Data.instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.instanceBuffer);
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