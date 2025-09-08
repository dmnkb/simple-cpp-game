#include "Renderer.h"
#include "RendererAPI.h"
#include "core/Profiler.h"
#include "pch.h"

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

void Renderer::update(Scene& scene)
{
    scene.setActiveCamera(scene.getDefaultCamera());

    Engine::Profiler::beginRegion("Shadow Pass");
    auto shadows = m_shadowPass.execute(scene);
    Engine::Profiler::endRegion("Shadow Pass");

    // Lighting pass inputs
    LightingInputs litIn{};
    litIn.shadows.spotShadowArray = shadows.spotShadowArray; // may be nullptr if no lights
    litIn.shadows.layers = shadows.layers;
    litIn.shadows.resolution = shadows.resolution;

    Engine::Profiler::beginRegion("Lighting Pass");
    m_lightingPass.execute(scene, litIn);
    Engine::Profiler::endRegion("Lighting Pass");

    Engine::Profiler::beginRegion("PostFX Pass");
    m_postProcessingPass.execute(m_lightingPass.getRenderTargetTexture());
    Engine::Profiler::endRegion("PostFX Pass");
}

} // namespace Engine
