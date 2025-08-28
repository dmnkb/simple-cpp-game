#include "ShadowPass.h"
#include "RendererAPI.h"
#include "core/Profiler.h"
#include "pch.h"
#include "scene/Scene.h"

namespace Engine
{

ShadowPass::ShadowPass() : m_depthShader("shader/depth.vs", "shader/depth.fs") {}

void ShadowPass::execute(Scene& scene)
{
    // at start of per-light shadow render
    glEnable(GL_DEPTH_TEST);
    // Disabled for rendering debug textures
    // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // Iterate spot lights directly
    const auto spotLights = scene.getSpotLights();

    int lightIndex = 0;
    for (const auto& light : spotLights)
    {
        const auto& shadowCam = light->getShadowCam();

        // Bind the light's shadow framebuffer
        const auto frameBuffer = light->getShadowFrameBuffer();
        frameBuffer->bind();

        // clear depth
        glClear(GL_DEPTH_BUFFER_BIT);

        const glm::mat4 lightSpaceMatrix = shadowCam->getProjectionMatrix() * shadowCam->getViewMatrix();

        // Bind once per light — lightSpaceMatrix is constant for all draws targeting this shadow map
        m_depthShader.bind();
        m_depthShader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

        // Render all objects that participate in shadow casting for this light
        const std::string rqName = "Shadow Pass (Light " + std::to_string(lightIndex) + ")";
        for (const auto& [material, meshMap] : scene.getRenderQueue(rqName))
        {

            if (!material->isDoubleSided)
            {
                glCullFace(GL_FRONT);
            }

            // Alpha-tested casters: depth shader samples diffuseMap for discard
            if (auto diffuse = material->getDiffuseMap())
            {
                diffuse->bind(0);
                m_depthShader.setUniform1i("diffuseMap", 0);
            }

            for (const auto& [mesh, transforms] : meshMap)
            {
                RendererAPI::drawInstanced(mesh, transforms);
                Profiler::registerDrawCall("Shadow Pass");
            }

            if (!material->isDoubleSided)
            {
                glCullFace(GL_BACK);
            }
        }

        m_depthShader.unbind();
        light->getShadowFrameBuffer()->unbind();

        ++lightIndex;
    }

    // enable when not rendering debug textures
    // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

} // namespace Engine
