#pragma once

#include "LightingPass.h"
#include "PostProcessingPass.h"
#include "Scene.h"
#include "ShadowPass.h"
#include "pch.h"

// TODO: Check the frame time, benchmark, The Cherno's Sponsa with Shadows; ~8 ms

// Scene
//  └── Stores Game Objects (Meshes, Lights, Player Cam, Sun, Environment Map)

// Renderer
//  ├── Stores Render Passes (ShadowPass, GBufferPass, ForwardPass, etc.)
//  └── Calls RenderPass::execute() for each pass

// RenderPass (Abstract Base Class)
//  ├── virtual void execute() = 0;
//  ├── Implements camera setup & FBO binding
//  ├── Fetches renderables for specific render flags (e.g., "Only Opaque" or "Only Shadows")
//  └── Delegates draw calls → RenderAPI

// RenderAPI
//  ├── Issues draw calls
//  ├── Abstracts API-specific state changes
//  └── Provides draw functions (drawInstanced, drawIndexed, etc.)

// Renderer orchestrates the rendering pipeline, but individual Render Passes control their own execution.

enum class ERenderPass
{
    Shadow,
    Lighting,
    PostProcessing,
    MaxElements
};

constexpr size_t RenderPassCount = static_cast<size_t>(ERenderPass::MaxElements);

class Renderer
{
  public:
    Renderer();
    ~Renderer();

    void update(Scene& scene);

    std::array<int, RenderPassCount> getDrawCallCounts()
    {
        return m_drawCallCounts;
    }

  private:
    ShadowPass m_shadowPass;
    LightingPass m_lightingPass;
    PostProcessingPass m_postProcessingPass;

    // Debug
    std::array<int, RenderPassCount> m_drawCallCounts;
};