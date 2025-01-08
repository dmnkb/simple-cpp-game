#pragma once

#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "RenderPass.h"
#include "Shader.h"

#define MAX_MESH_COUNT 256
#define MAX_TRANSFORMS_PER_MESH 256

// Refactor idea:
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

/** Current:
 * Shader
 * L   Mesh (sorted from far to near)
 *     L    std::vector<glm::mat4> transforms
 */
using RenderQueue = std::unordered_map<Ref<Shader>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

// TODO: Future idea:
// RenderQueue
// ├── Mesh1
// │   ├── SubMesh1 (Material1)
// │   │   ├── InstanceDataBuffer1
// │   │   │   ├── mat4 Transform1
// │   │   │   ├── AnimationState1
// │   │   │   ├── AnimationWeights1
// │   │   │   └── ...
// │   ├── SubMesh2 (Material2)
// │   │   └── InstanceDataBuffer1
// │   │       ├── mat4 Transform1
// │   │       ├── AnimationState1
// │   │       └── ...
// │   └── ...
// ├── Mesh2
// │   ├── SubMesh1 (Material1)
// │   │   └── InstanceDataBuffer1
// │   └── ...
// └── ...

struct ShadowCaster
{
    glm::mat4 lightSpaceMatrix;
    Texture depthTexture;
};

struct RendererData
{
    GLuint instanceBuffer;
    GLuint uboLights;
    std::vector<ShadowCaster> shadowCasters;
};

struct RenderStats
{
    int drawCallCount = 0;
};
class Renderer
{
  public:
    static void init();
    static void update();

    const static RenderStats& getStats();
    const static void resetStats();

  private:
    static void drawQueue(const RenderQueue& queue);
    static void drawInstanceBatch(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms);
    static void bindInstanceBuffer(const std::vector<glm::mat4>& transforms);
    static void unbindInstancBuffer();
    static void prepareLightingUBO();

    // Debugging
    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
    static void enableOpenGLDebugOutput();
};