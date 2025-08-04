#include "ShadowPass.h"
#include "RendererAPI.h"
#include "Scene.h"

ShadowPass::ShadowPass() : m_depthShader("shader/depth.vs", "shader/depth.fs") {}

void ShadowPass::execute(Scene& scene, int& drawCalls)
{
    int lightIndex = 0;
    for (auto& lightSceneNode : scene.getLightSceneNodes())
    {
        lightIndex++;
        const auto lightType = lightSceneNode->getLightType();
        if (lightType == ELT_POINT)
        {
            // Point lights don't cast shadows (yet)
            continue;
        }
        else
        {
            // Spotlights or directional Light
            const auto& shadowCam = lightSceneNode->getShadowCam();

            // Bind the light's framebuffer in order to render to it
            lightSceneNode->getShadowFrameBuffer()->bind();

            const auto lightSpaceMatrix = shadowCam->getProjectionMatrix() * shadowCam->getViewMatrix();

            for (const auto& [material, meshMap] :
                 scene.getRenderQueue("Shadow Pass (Light " + std::to_string(lightIndex) + ")"))
            {
                if (!material->isDoubleSided)
                {
                    glCullFace(GL_FRONT);
                }

                m_depthShader.bind();
                material->getDiffuseMap()->bind(0);
                m_depthShader.setUniform1i("diffuseMap", 0);
                m_depthShader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

                for (const auto& [mesh, transforms] : meshMap)
                {
                    RendererAPI::drawInstanced(mesh, transforms);
                    drawCalls++;
                }

                if (!material->isDoubleSided)
                {
                    glCullFace(GL_BACK);
                }

                m_depthShader.unbind();
            }
            lightSceneNode->getShadowFrameBuffer()->unbind();
        }
    }
}
