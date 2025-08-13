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
    static int drawCallCount = 0;

    Engine::Profiler::beginRegion("Shadow Pass");
    m_shadowPass.execute(scene, drawCallCount);
    m_drawCallCounts[static_cast<size_t>(ERenderPass::Shadow)] = drawCallCount;
    drawCallCount = 0;
    Engine::Profiler::endRegion("Shadow Pass");

    Engine::Profiler::beginRegion("Lighting Pass");
    m_lightingPass.execute(scene, drawCallCount);
    m_drawCallCounts[static_cast<size_t>(ERenderPass::Lighting)] = drawCallCount;
    drawCallCount = 0;
    Engine::Profiler::endRegion("Lighting Pass");

    Engine::Profiler::beginRegion("PostFX Pass");
    m_postProcessingPass.execute(scene, m_lightingPass.getRenderTargetTexture());
    m_drawCallCounts[static_cast<size_t>(ERenderPass::PostProcessing)] = drawCallCount;
    Engine::Profiler::endRegion("PostFX Pass");
}

} // namespace Engine