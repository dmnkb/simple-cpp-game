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
    m_shadowPass.execute(scene);
    Engine::Profiler::endRegion("Shadow Pass");

    Engine::Profiler::beginRegion("Lighting Pass");
    m_lightingPass.execute(scene);
    Engine::Profiler::endRegion("Lighting Pass");

    Engine::Profiler::beginRegion("PostFX Pass");
    m_postProcessingPass.execute(m_lightingPass.getRenderTargetTexture());
    Engine::Profiler::endRegion("PostFX Pass");
}

} // namespace Engine