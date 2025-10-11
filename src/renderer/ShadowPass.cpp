#include "ShadowPass.h"
#include "PassIO.h"
#include "RendererAPI.h"
#include "core/Profiler.h"
#include "pch.h"
#include "scene/Scene.h"

namespace Engine
{

ShadowPass::ShadowPass()
    : m_depthShader("shader/depth.vs", "shader/depth.fs"),
      m_depthShaderCube("shader/depthCube.vs", "shader/depthCube.fs")
{
    setupSpotLightRessources();
    setupPointLightRessources();
}

// MARK: Execute
ShadowOutputs ShadowPass::execute(Scene& scene)
{
    ShadowOutputs out{};

    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    renderSpotLights(scene, scene.getSpotLights());
    renderPointLights(scene, scene.getPointLights());

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    out.spotShadowArray = m_spotLightDepthArray;
    out.pointShadowCubeArray = m_pointLightDepthCubeArray;
    return out;
}

// MARK: Render spot lights
void ShadowPass::renderSpotLights(Scene& scene, const std::vector<Ref<SpotLight>>& spotLights)
{
    m_depthShader.bind();

    const int count = static_cast<int>(spotLights.size());
    if (count <= 0)
        return;

    for (int lightIndex = 0; lightIndex < count; ++lightIndex)
    {
        const auto& light = spotLights[lightIndex];

        m_spotLightShadowFramebuffer->reattachLayerForAll(lightIndex);
        m_spotLightShadowFramebuffer->bind();

        glViewport(0, 0, m_shadowRes, m_shadowRes);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);

        const auto& cam = light->getShadowCam();
        const glm::mat4 matrix = cam->getProjectionMatrix() * cam->getViewMatrix();
        m_depthShader.setUniformMatrix4fv("lightSpaceMatrix", matrix);

        const std::string rqName = "Shadow Pass (Spot light " + std::to_string(lightIndex) + ")";
        for (const auto& [material, meshMap] : scene.getRenderQueue(rqName))
        {
            if (!material->isDoubleSided)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
            }

            if (auto diffuse = material->getDiffuseMap())
            {
                if (m_depthShader.hasUniform("diffuseMap")) // <- FIX: was checking m_depthShaderCube
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
                glCullFace(GL_BACK);
        }

        m_spotLightShadowFramebuffer->unbind();
    }

    m_depthShader.unbind();
}

// MARK: Render point lights
void ShadowPass::renderPointLights(Scene& scene, const std::vector<Ref<PointLight>>& pointLights)
{
    m_depthShaderCube.bind();

    const int count = static_cast<int>(pointLights.size());
    if (count <= 0)
        return;

    m_pointLightShadowFramebuffer->bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Canonical cube face directions & ups (must match your PointLight)
    static const glm::vec3 kDir[6] = {{+1, 0, 0}, {-1, 0, 0}, {0, +1, 0}, {0, -1, 0}, {0, 0, +1}, {0, 0, -1}};
    static const glm::vec3 kUp[6] = {{0, -1, 0}, {0, -1, 0}, {0, 0, +1}, {0, 0, -1}, {0, -1, 0}, {0, -1, 0}};

    for (int li = 0; li < count; ++li)
    {
        const auto& light = pointLights[li];
        const glm::vec3 lpos = light->getPointLightProperties().position;
        const float nearP = 0.05f;
        const float farP = light->getRange(); // must match lighting pass p_ranges[li].x
        const glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, nearP, farP);
        const int baseLayer = li * 6;

        for (int face = 0; face < 6; ++face)
        {
            // Attach the correct array layer for this (light,face)
            m_pointLightShadowFramebuffer->reattachLayerForAll(baseLayer + face);

            glViewport(0, 0, m_pointShadowRes, m_pointShadowRes);
            glClear(GL_DEPTH_BUFFER_BIT);

            // Build per-face view matrix
            const glm::mat4 view = glm::lookAt(lpos, lpos + kDir[face], kUp[face]);

            // Set uniforms expected by depthCube.vs/fs
            m_depthShaderCube.setUniformMatrix4fv("u_View", view);
            m_depthShaderCube.setUniformMatrix4fv("u_Proj", proj);
            m_depthShaderCube.setUniform3fv("uLightPosWS", lpos);
            m_depthShaderCube.setUniformFloat("uShadowFar", farP);

            const std::string rqName = "Shadow Pass (Point light " + std::to_string(li) + ")";
            for (const auto& [material, meshMap] : scene.getRenderQueue(rqName))
            {
                if (!material->isDoubleSided)
                {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                }

                if (auto diffuse = material->getDiffuseMap())
                {
                    if (m_depthShaderCube.hasUniform("diffuseMap"))
                    {
                        diffuse->bind(0);
                        m_depthShaderCube.setUniform1i("diffuseMap", 0); // <- FIX: use m_depthShaderCube
                    }
                }

                for (const auto& [mesh, transforms] : meshMap)
                {
                    RendererAPI::drawInstanced(mesh, transforms);
                    Profiler::registerDrawCall("Shadow Pass");
                }

                if (!material->isDoubleSided)
                    glCullFace(GL_BACK);
            }
        }
    }

    m_pointLightShadowFramebuffer->unbind();
    m_depthShaderCube.unbind();
}

// MARK: Setup spot ressources
void ShadowPass::setupSpotLightRessources()
{
    m_spotLightShadowFramebuffer = CreateRef<Framebuffer>();

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

    m_spotLightDepthArray = CreateRef<Texture>(props, customProps);
    m_spotLightShadowFramebuffer->attachTexture(m_spotLightDepthArray);

    std::cout << "Spot light shadow array: " << m_spotLightDepthArray->properties.width << "x"
              << m_spotLightDepthArray->properties.height << " layers=" << m_spotLightDepthArray->properties.layers
              << "\n";
}

// MARK: Setup point ressources
void ShadowPass::setupPointLightRessources()
{
    m_pointLightShadowFramebuffer = CreateRef<Framebuffer>();

    TextureProperties props{};
    props.target = GL_TEXTURE_CUBE_MAP_ARRAY;
    props.level = 0;
    props.width = m_pointShadowRes;
    props.height = m_pointShadowRes;
    props.layers = MAX_POINTLIGHT_SHADOW_COUNT * 6;
    props.internalFormat = GL_DEPTH_COMPONENT24;
    props.format = GL_DEPTH_COMPONENT;
    props.type = GL_FLOAT;
    props.pixels = nullptr;

    CustomProperties custom{};
    custom.mipmaps = false;
    custom.minFilter = GL_LINEAR;
    custom.magFilter = GL_LINEAR;
    custom.wrapS = GL_CLAMP_TO_EDGE;
    custom.wrapT = GL_CLAMP_TO_EDGE;
    custom.wrapR = GL_CLAMP_TO_EDGE;
    custom.attachmentType = GL_DEPTH_ATTACHMENT;
    custom.attachLayer = 0;

    m_pointLightDepthCubeArray = CreateRef<Texture>(props, custom);

    // default attach to layer 0; reattach per face with glFramebufferTextureLayer during rendering
    m_pointLightShadowFramebuffer->attachTexture(m_pointLightDepthCubeArray);

    std::cout << "Point light shadow cube array: " << m_pointLightDepthCubeArray->properties.width << "x"
              << m_pointLightDepthCubeArray->properties.height << " cubes=" << MAX_POINTLIGHT_SHADOW_COUNT
              << " layers=" << m_pointLightDepthCubeArray->properties.layers << "\n";
}

} // namespace Engine
