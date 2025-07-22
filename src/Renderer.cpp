#include "Renderer.h"
#include "RendererAPI.h"

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

    m_shadowPass.execute(scene, drawCallCount);
    m_drawCallCounts[static_cast<size_t>(ERenderPass::Shadow)] = drawCallCount;
    drawCallCount = 0;

    m_lightingPass.execute(scene, drawCallCount);
    m_drawCallCounts[static_cast<size_t>(ERenderPass::Lighting)] = drawCallCount;
    drawCallCount = 0;

    m_postProcessingPass.execute(scene, m_lightingPass.getRenderTargetTexture());
    m_drawCallCounts[static_cast<size_t>(ERenderPass::PostProcessing)] = drawCallCount;
}
