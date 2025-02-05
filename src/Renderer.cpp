#include "Renderer.h"
#include "RendererAPI.h"

// TODO: Check the frame time, benchmark, The Cherno's Sponsa with Shadows; ~8 ms

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
