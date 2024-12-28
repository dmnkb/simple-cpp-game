#pragma once

#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "RenderPass.h"
#include "Shader.h"

#define MAX_MESH_COUNT 256
#define MAX_TRANSFORMS_PER_MESH 256

// TODO: Refactor goal:
// Renderer
//     Manages the overall pipeline and orchestration.
//     Delegates tasks to managers and passes.
// RenderPassManager
//     Stores and manages all RenderPass instances.
//     Handles pass execution, input/output dependencies, and framebuffer bindings.
// LightingManager
//     Manages lights and UBO updates.
// RenderQueue
//     Tracks opaque and transparent objects.
//     Provides sorting utilities.
// RenderStats
//     Extended to include more detailed performance metrics.

// An atomic, stateless POD (plain old data) that contains mesh, material and transform.
struct Renderable
{
    Ref<Mesh> mesh; // Geometry data
    // Ref<Material> material; // TODO: Shader, textures, material properties
    Ref<Shader> shader; // TODO: replace with material
    glm::mat4 transform;

    // TODO: For more flexibility in the future, render flags that are filterable might
    // scale better, than relying on different render queues (transparent, opaque).
    // uint32_t renderFlags; // Bitmask of properties, e.g., OPAQUE, TRANSPARENT, SHADOW_CASTER
    // using:
    // std::function<bool(const Renderable&)> customFilter = [](const Renderable& renderable)
    // { return renderable.isOpaque() && renderable.castsShadow(); };
};

enum ERenderQueueFilter
{
    ALL,
    OPAQUE,
    TRANSPARENT,
    QUAD
};

using RenderQueue = std::unordered_map<Ref<Shader>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

/**
 * transparent
 * L    Material
 *      L   Mesh (sorted from far to near)
 *          L vector<glm::mat4>
 * opaque
 * L    Material
 *      L   Mesh (sorted from far to near)
 *          L vector<glm::mat4>
 * TODO: Might be refactored later to use render flags (see above) instead of separate queues
 */
struct RenderQueueData
{
    RenderQueue transparent;
    RenderQueue opaque;
};

// TODO: This data actually belongs INSIDE the pass class
struct RenderPassAndFilter
{
    RenderPass pass;
    ERenderQueueFilter filter = ERenderQueueFilter::ALL;
    Ref<Camera> camera = nullptr;
    std::vector<Texture> resultTextures = {};
    std::string name = "";
};

struct RendererData
{
    RenderQueueData renderQueue;
    GLuint instanceBuffer;

    std::vector<Light> lights;
    GLuint uboLights;

    Ref<Camera> defaultCamera;
    Ref<Camera> activeCamera;
    std::vector<RenderPassAndFilter> renderPasses;
};

struct RenderStats
{
    int drawCallCount = 0;
};
class Renderer
{
  public:
    static void init(const Ref<Camera>& camera);

    // Setup viewMatrix & viewProjectionMatrix
    static void beginScene(const Ref<Camera>& camera);
    static void update();

    static void submitLight(const Light& lights);
    static void submitRenderable(const Renderable& renderable);

    const static RenderStats& getStats();
    const static void resetStats();

  private:
    static void drawAll(const ERenderQueueFilter& filter, const Ref<Shader> overrideShader = nullptr);
    static void drawQueue(const RenderQueue& queue);
    static void drawBatch(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms);
    static void prepareInstanceBuffer();
    static void bindInstanceData(const std::vector<glm::mat4>& transforms);
    static void unbindInstancBuffer();
    static void prepareLighting();
    static void updateCameraFromLight(Ref<Camera>& camera, const Light& light);
    static void drawFullScreenQuad();

    // TODO:
    // void setActiveCamera(Camera* camera) {
    //     m_activeCamera = camera;
    // }

    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
    static void enableOpenGLDebugOutput();
};