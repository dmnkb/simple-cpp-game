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
}

ShadowOutputs ShadowPass::execute(Scene& scene)
{
    ShadowOutputs out{};

    // Gather spot lights that cast shadows
    const auto spotLights = scene.getSpotLights();
    const int numSpots = static_cast<int>(spotLights.size());
    if (numSpots <= 0)
        return out; // nothing to render, return empty

    // Ensure FBO/array textures are sized for current light count
    ensureFBOForSpotCount(numSpots);

    glEnable(GL_DEPTH_TEST);

    // Save current viewport and set to shadow-map resolution
    GLint prevViewport[4];
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    glViewport(0, 0, m_shadowRes, m_shadowRes);

    m_depthShader.bind();

    const bool hasDiffuseMap = m_depthShader.hasUniform("diffuseMap");

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    int lightIndex = 0;
    for (const auto& light : spotLights)
    {
        // Reattach FBO to the proper slice (per-layer re-attach)
        m_shadowFramebuffer->reattachLayerForAll(lightIndex);
        GLenum s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (s != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "FBO not complete for layer " << lightIndex << "\n";
        m_shadowFramebuffer->bind();

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);

        const auto& shadowCam = light->getShadowCam();
        const glm::mat4 lightSpaceMatrix = shadowCam->getProjectionMatrix() * shadowCam->getViewMatrix();
        m_depthShader.setUniformMatrix4fv("lightSpaceMatrix", lightSpaceMatrix);

        // Render all shadow casters for this light
        const std::string rqName = "Shadow Pass (Light " + std::to_string(lightIndex) + ")";
        for (const auto& [material, meshMap] : scene.getRenderQueue(rqName))
        {
            if (!material->isDoubleSided)
            {
                // reduce peter-panning
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
                Profiler::registerDrawCall("Shadow Pass");
            }

            if (!material->isDoubleSided)
            {
                glCullFace(GL_BACK);
            }
        }

        m_shadowFramebuffer->unbind();
        ++lightIndex;
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Restore previous viewport
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

    m_depthShader.unbind();

    out.spotShadowArray = m_depthArray;
    out.layers = m_allocLayers;
    out.resolution = m_shadowRes;

    return out;
}

void ShadowPass::ensureFBOForSpotCount(int spotCount)
{
    const int layers = std::min(spotCount, MAX_SPOTLIGHT_SHADOW_COUNT);
    // If first run or layer count changed, recreate
    if (m_allocLayers != layers || !m_depthArray)
    {
        recreateFBO(layers, m_shadowRes, m_shadowRes, false);
        m_allocLayers = layers;
    }
}

// FIXME: Upon rezising the view port, the framerate almost halfes. Sometimes the app crashes

void ShadowPass::recreateFBO(int layers, int width, int height, bool withDebug)
{
    // Recreate FBO object and attachments
    m_shadowFramebuffer = CreateRef<Framebuffer>();

    // Spot light depth array texture (GL_TEXTURE_2D_ARRAY)
    {
        TextureProperties props{};
        props.target = GL_TEXTURE_2D_ARRAY;
        props.level = 0;
        props.width = width;
        props.height = height;
        props.layers = layers;
        props.internalFormat = GL_DEPTH_COMPONENT24;
        props.format = GL_DEPTH_COMPONENT;
        props.type = GL_FLOAT;
        props.pixels = nullptr; // allocate only

        CustomProperties cprops{};
        cprops.mipmaps = false;
        cprops.minFilter = GL_LINEAR;
        cprops.magFilter = GL_LINEAR;
        cprops.wrapS = GL_CLAMP_TO_BORDER;
        cprops.wrapT = GL_CLAMP_TO_BORDER;
        cprops.wrapR = GL_CLAMP_TO_BORDER;
        cprops.attachmentType = GL_DEPTH_ATTACHMENT;
        cprops.attachLayer = 0; // default slice when first attached

        m_depthArray = CreateRef<Texture>(props, cprops);

        // Enable hardware depth compare for sampler2DArrayShadow
        m_depthArray->bind(0);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        const float border[4] = {1.f, 1.f, 1.f, 1.f};
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border);
        m_depthArray->unbind(0);

        m_shadowFramebuffer->attachTexture(m_depthArray);
    }

    std::cout << "Shadow array: " << m_depthArray->properties.width << "x" << m_depthArray->properties.height
              << " layers=" << m_depthArray->properties.layers << "\n";
}

} // namespace Engine
