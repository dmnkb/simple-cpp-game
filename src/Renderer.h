#pragma once

#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderPass.h"
#include "TextureManager.h"

// Status Quo:
// RenderQueue
// ├── Material
// │   ├── Mesh1
// │   │   ├── mat4 Transform1
// │   │   ├── mat4 Transform2
// │   │   └── ...
// │   ├── Mesh2
// │   │   ├── mat4 Transform1
// │   │   ├── mat4 Transform2
// │   │   └── ...

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

using RenderQueue = std::unordered_map<Ref<Material>, std::unordered_map<Ref<Mesh>, std::vector<glm::mat4>>>;

struct RendererData
{
    GLuint instanceBuffer;
    GLuint uboLights;

    // TODO: This could be combined such that both is part of a vector of Lights
    struct ShadowCaster
    {
        glm::mat4 lightSpaceMatrix;
        Ref<Texture> depthTexture;
    };
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
    static void shutdown();

    const static RenderStats& getStats();
    const static void resetStats();
    const static std::vector<Ref<Texture>> getShadowCasterDepthBuffers();

  private:
    static void executeShadowPass(const RenderQueue& queue);
    static void executePass(const RenderQueue& queue);
    static void drawInstanceBatch(const Ref<Mesh>& mesh, const std::vector<glm::mat4>& transforms);
    static void bindInstanceBuffer(const std::vector<glm::mat4>& transforms);
    static void unbindInstancBuffer();
    static void prepareLightingUBO();

    // Debugging
    static void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);
    static void enableOpenGLDebugOutput();
};