#include "ShadowPass.h"
#include "Framebuffer.h"
#include "RendererAPI.h"
#include "Scene.h"
#include "Shader.h"
#include "Window.h"

ShadowPass::ShadowPass()
    : m_depthShader("shader/depth.vs", "shader/depth.fs"),
      m_depthShaderFoliage("shader/depth-foliage.vs", "shader/depth.fs")
{
}

void ShadowPass::execute()
{

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

            lightSceneNode->getFrameBuffer()->bind();
            const auto lightSpaceMatrix = shadowCam->getProjectionMatrix() * shadowCam->getViewMatrix();

            // for (const auto& [material, meshMap] :
            //      Scene::getRenderQueue([](const Ref<MeshSceneNode>& node) { return true; }))
            for (const auto& [material, meshMap] : Scene::getRenderQueue())
            {
                glEnable(GL_CULL_FACE);
                if (material->name == "foliage")
                {
                    glCullFace(GL_BACK);
                    // TODO: Pre-sort scene for foliage, such that each shader only has to be bound once
                    m_depthShaderFoliage.bind();
                    material->getDiffuseMap()->bind(0);
                    m_depthShaderFoliage.setUniform1i("diffuseMap", 0);
                    m_depthShaderFoliage.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
                    m_depthShaderFoliage.setUniform1f("u_Time", Window::getElapsedTime());
                }
                else
                {
                    glCullFace(GL_FRONT);
                    m_depthShader.bind();
                    material->getDiffuseMap()->bind(0);
                    m_depthShader.setUniform1i("diffuseMap", 0);
                    m_depthShader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);
                }

                for (const auto& [mesh, transforms] : meshMap)
                {
                    RendererAPI::drawInstanced(mesh, transforms);
                }

                glCullFace(GL_BACK);
                if (material->name == "foliage")
                {
                    m_depthShaderFoliage.unbind();
                }
                else
                {
                    m_depthShader.unbind();
                }
            }
        }
    }
}
