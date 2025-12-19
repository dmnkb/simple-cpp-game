#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <fmt/core.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/Profiler.h"
#include "core/Window.h"
#include "pch.h"
#include "renderer/DirectionalLight.h"
#include "renderer/Framebuffer.h"
#include "renderer/GLDebug.h"
#include "renderer/LightingPass.h"
#include "renderer/RendererAPI.h"
#include "scene/Scene.h"

namespace Engine
{

LightingPass::LightingPass() {}

LightingPass::~LightingPass() {}

// MARK: Execute
LightingOutputs LightingPass::execute(const Ref<Scene>& scene, const LightingInputs& lightInputs)
{
    LightingOutputs out{};

    GLCall(glColorMask(true, true, true, true));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCall(glViewport(0, 0, Window::frameBufferDimensions.x, Window::frameBufferDimensions.y));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));

    for (const auto& [material, submeshBatchMap] : scene->getRenderQueue("Lighting Pass"))
    {
        material->bind();
        material->update();

        const auto& cam = scene->getActiveCamera();
        const auto vp = cam.getProjectionMatrix() * cam.getViewMatrix();
        const auto camPos = cam.getPosition();
        const auto ambientColor = scene->getAmbientLightColor();

        material->setUniformMatrix4fv("uViewProjection", vp);
        material->setUniform3fv("uViewPos", camPos);
        material->setUniform4fv("uAmbientLightColor", ambientColor);

        for (const auto& [submeshKey, transforms] : submeshBatchMap)
        {
            RendererAPI::drawInstanced(submeshKey.mesh, submeshKey.submeshIndex, transforms);
            Profiler::registerDrawCall("Lighting Pass");
        }

        material->unbind();
    }

    return out;
}

} // namespace Engine
