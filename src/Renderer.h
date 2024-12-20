#pragma once

#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "RenderPass.h"
#include "Shader.h"

#define MAX_MESH_COUNT 256
#define MAX_TRANSFORMS_PER_MESH 256

// An atomic, stateless POD (plain old data) that contains mesh, material and transform.
struct Renderable
{
    Ref<Mesh> mesh; // Geometry data
    // Ref<Material> material; // TODO: Shader, textures, material properties
    Ref<Shader> shader; // TODO: replace with material
    glm::mat4 transform;
};

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

enum ERenderQueueFilter
{
    ALL,
    OPAQUE,
    TRANSPARENT
};

using RenderQueue = std::unordered_map<Ref<Shader>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

struct RenderQueueData
{
    RenderQueue transparent;
    RenderQueue opaque;
};

struct RenderPassAndFilter
{
    RenderPass pass;
    ERenderQueueFilter filter;
};

struct RendererData
{
    glm::mat4 viewProjectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec3 camPos;

    RenderQueueData renderQueue;
    GLuint instanceBuffer;

    std::vector<Light> lights[256];
    GLuint uboLights;

    std::vector<RenderPassAndFilter> renderPasses;
};

struct RenderStats
{
    int drawCallCount = 0;
};
class Renderer
{
  public:
    static void init();

    // Setup viewMatrix & viewProjectionMatrix
    static void beginScene(Camera& camera);
    static void update(const glm::vec2& windowDimensions);

    static void submitLight(const Light& lights);
    static void submitRenderable(const Renderable& renderable);

    const static RenderStats& getStats();
    const static void resetStats();

  private:
    static void drawAll(const ERenderQueueFilter& filter);
    static void drawQueue(const RenderQueue& queue);
    static void drawBatch(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms);
    static void prepareInstanceBuffer();
    static void bindInstanceData(const std::vector<glm::mat4>& transforms);
    static void unbindInstancBuffer();
    static void prepareLighting();

    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
    static void enableOpenGLDebugOutput();
};