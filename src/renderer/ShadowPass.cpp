#include "ShadowPass.h"
#include "PassIO.h"
#include "RendererAPI.h"
#include "core/Profiler.h"
#include "pch.h"
#include "scene/Scene.h"

namespace Engine
{

ShadowPass::ShadowPass() : m_depthShader("shader/depth.vs", "shader/depth.fs")
{
    m_shadowFramebuffer = CreateRef<Framebuffer>();

    TextureProperties props{};
    props.target = GL_TEXTURE_2D_ARRAY;
    props.level = 0;
    props.width = m_shadowRes;
    props.height = m_shadowRes;
    props.layers = MAX_SPOTLIGHT_SHADOW_COUNT;
    props.internalFormat = GL_DEPTH_COMPONENT24;
    props.format = GL_DEPTH_COMPONENT;
    props.type = GL_FLOAT;
    props.pixels = nullptr;

    CustomProperties customProps{};
    customProps.mipmaps = false;
    customProps.minFilter = GL_LINEAR;
    customProps.magFilter = GL_LINEAR;
    customProps.wrapS = GL_CLAMP_TO_BORDER;
    customProps.wrapT = GL_CLAMP_TO_BORDER;
    customProps.wrapR = GL_CLAMP_TO_BORDER;
    customProps.attachmentType = GL_DEPTH_ATTACHMENT;
    customProps.attachLayer = 0;

    m_depthArray = CreateRef<Texture>(props, customProps);

    // Needed for hardware compare
    m_depthArray->bind(0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    m_depthArray->unbind(0);

    m_shadowFramebuffer->attachTexture(m_depthArray);

    std::cout << "Shadow array: " << m_depthArray->properties.width << "x" << m_depthArray->properties.height
              << " layers=" << m_depthArray->properties.layers << "\n";
}

ShadowOutputs ShadowPass::execute(Scene& scene)
{
    ShadowOutputs out{};

    // MARK: Get spot lights
    const auto spotLights = scene.getSpotLights();
    const int spotLightCount = static_cast<int>(spotLights.size());

    if (spotLightCount <= 0)
        return out;

    glEnable(GL_DEPTH_TEST);

    m_depthShader.bind();

    const bool hasDiffuseMap = m_depthShader.hasUniform("diffuseMap");
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // MARK: [FOR EACH: SPOT L]
    for (int lightIndex = 0; lightIndex < spotLightCount; ++lightIndex)
    {
        const auto& light = spotLights[lightIndex];

        // MARK: > Bind FB
        m_shadowFramebuffer->reattachLayerForAll(lightIndex);
        m_shadowFramebuffer->bind();
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);

        const auto& shadowCam = light->getShadowCam();
        const glm::mat4 lightSpaceMatrix = shadowCam->getProjectionMatrix() * shadowCam->getViewMatrix();
        m_depthShader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

        const std::string rqName = "Shadow Pass (Light " + std::to_string(lightIndex) + ")";
        for (const auto& [material, meshMap] : scene.getRenderQueue(rqName))
        {
            if (!material->isDoubleSided)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
            }

            if (hasDiffuseMap)
            {
                if (auto diffuse = material->getDiffuseMap())
                {
                    diffuse->bind(0);
                    m_depthShader.setUniform1i("diffuseMap", 0);
                }
            }

            for (const auto& [mesh, transforms] : meshMap)
            {
                RendererAPI::drawInstanced(mesh, transforms);
                // MARK: > Draw
                Profiler::registerDrawCall("Shadow Pass");
            }

            if (!material->isDoubleSided)
                glCullFace(GL_BACK);
        }

        // MARK: > Unbind FB
        m_shadowFramebuffer->unbind();
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    m_depthShader.unbind();

    out.spotShadowArray = m_depthArray;
    return out;
}

} // namespace Engine
