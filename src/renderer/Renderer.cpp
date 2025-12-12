#include "renderer/Renderer.h"
#include "core/Profiler.h"
#include "pch.h"
#include "renderer/PassIO.h"
#include "renderer/RendererAPI.h"

namespace Engine
{

Renderer::Renderer()
{
    RendererAPI::init();
}

Renderer::~Renderer()
{
    RendererAPI::shutdown();
}

// TODO: New input

// View description coming from Editor (orbit cam, fly cam, etc.)
// struct RenderView
// {
//     Mat4 view;
//     Mat4 proj;
//     Frustum frustum;
//     Vec2 viewportSize;
//     Vec2 jitter = {};
//     float exposure = 1.0f;
//     bool isEditorView = true; // optional
// };

// Transient, render-friendly world after extraction from ECS.
// struct RenderWorld {
//   // flat, tightly-packed arrays (handles + transforms + material ids etc.)
//   std::span<const StaticMeshInstance> statics;
//   std::span<const SkinnedMeshInstance> skinned;
//   std::span<const DecalInstance> decals;
//   std::span<const Light> lights;            // includes sun dir/light
//   SceneBounds worldBounds;
//   // pointers/handles to GPU-resident geometry/material tables
//   GpuScene* gpuScene = nullptr;
// };

// // What the renderer needs each frame:
// struct RenderInputs {
//   RenderWorld world;                  // extracted once per frame
//   std::span<const RenderView> views;  // one or many (main, thumbnails, debug)
//   CSMDebugSettings csmDebug;          // optional
//   SkySettings sky;
//   // references to persistent renderer state (history buffers, blue noise, etc.)
// };

// // Renderer interface
// class Renderer {
// public:
//   void beginFrame(FrameContext& ctx);
//   void renderFrame(const RenderInputs& in, FrameContext& ctx); // cull+record+execute
//   void endFrame(FrameContext& ctx);
// };

void Renderer::render(const Ref<Scene>& scene)
{
    // Engine::Profiler::beginGPURegion("Shadow Pass");
    // Engine::Profiler::beginCPURegion("Shadow Pass");
    // ShadowOutputs shadowOutputs = m_shadowPass.execute(scene);
    // Engine::Profiler::endCPURegion("Shadow Pass");
    // Engine::Profiler::endGPURegion("Shadow Pass");

    Engine::Profiler::beginGPURegion("Lighting Pass");
    Engine::Profiler::beginCPURegion("Lighting Pass");
    // LightingOutputs lightingOutputs = m_lightingPass.execute(scene, shadowOutputs);
    LightingOutputs lightingOutputs = m_lightingPass.execute(scene, {});
    Engine::Profiler::endCPURegion("Lighting Pass");
    Engine::Profiler::endGPURegion("Lighting Pass");

    // Engine::Profiler::beginGPURegion("PostFX Pass");
    // Engine::Profiler::beginCPURegion("PostFX Pass");
    // m_postProcessingPass.execute(lightingOutputs);
    // Engine::Profiler::endCPURegion("PostFX Pass");
    // Engine::Profiler::endGPURegion("PostFX Pass");

    // Swap GPU timer buffers to retrieve results for next frame
    Engine::Profiler::swapGPUTimerBuffers();
}

} // namespace Engine
