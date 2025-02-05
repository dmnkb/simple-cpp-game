#pragma once

#include "ForwardPass.h"
#include "RendererTypes.h"
#include "ShadowPass.h"
#include "pch.h"

// Scene
//  ├── Stores Game Objects (Meshes, Lights, Cameras)
//  └── Controls active camera (e.g., player, shadow pass, reflections, etc.)

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
class Renderer
{
  public:
    Renderer();
    ~Renderer();

    void update();

  private:
    ForwardPass m_forwardPass;
    ShadowPass m_shadowPass;
};