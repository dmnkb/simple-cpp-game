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
    m_shadowPass.execute(scene);
    m_lightingPass.execute(scene);
    m_postProcessingPass.execute(scene, m_lightingPass.getRenderTargetTexture());
}
