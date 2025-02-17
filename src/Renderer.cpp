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

void Renderer::update()
{
    m_shadowPass.execute();
    m_forwardPass.execute();
}
