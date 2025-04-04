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
    m_forwardPass.execute(scene);
}
