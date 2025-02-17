#include "ShadowPass.h"
#include "Framebuffer.h"
#include "RendererAPI.h"
#include "Scene.h"
#include "Shader.h"

ShadowPass::ShadowPass() : m_depthShader("assets/depth.vs", "assets/depth.fs") {}

void ShadowPass::execute()
{
    m_depthShader.bind();
    glCullFace(GL_FRONT);

    for (auto& lightSceneNode : Scene::getLightSceneNodes())
    {
        const auto lightType = lightSceneNode->getLightType();
        if (lightType == ELT_POINT)
        {
            // Point lights don't cast shadows
            continue;
        }
        else
        {
            // Spotlights or directional Light
            const auto& shadowCam = lightSceneNode->getShadowCam();

            lightSceneNode->getFrameBuffer()->bind(lightSceneNode->getDepthBuffer());
            const auto lightSpaceMatrix = shadowCam->getProjectionMatrix() * shadowCam->getViewMatrix();
            m_depthShader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

            for (const auto& [material, meshMap] :
                 Scene::getRenderQueue([](const Ref<MeshSceneNode>& node) { return true; }))
            {
                for (const auto& [mesh, transforms] : meshMap)
                {
                    RendererAPI::drawInstanced(mesh, transforms);
                }
            }
        }
    }

    glCullFace(GL_BACK);
    m_depthShader.unbind();
}
