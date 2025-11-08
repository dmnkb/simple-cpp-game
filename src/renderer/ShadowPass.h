#pragma once

#include "DirectionalLight.h"
#include "Framebuffer.h"
#include "PassIO.h"
#include "PointLight.h"
#include "Shader.h"
#include "SpotLight.h"
#include "Texture.h"
#include "scene/Scene.h"

namespace Engine
{
constexpr int MAX_SPOTLIGHT_SHADOW_COUNT = 16;
constexpr int MAX_POINTLIGHT_SHADOW_COUNT = 16;
constexpr int DIRECTIONAL_SHADOW_CASCADE_COUNT = 4;

class ShadowPass
{
  public:
    ShadowPass();
    ShadowOutputs execute(Scene& scene);

  private:
    void setupSpotLightRessources();
    void setupPointLightRessources();
    void setupDirectionalLightRessources();

    void renderSpotLights(Scene& scene, const std::vector<Ref<SpotLight>>& spotLights);
    void renderPointLights(Scene& scene, const std::vector<Ref<PointLight>>& pointLights);
    void renderDirectionalLight(Scene& scene, const Ref<DirectionalLight>& directionalLight);

  private:
    // Spot light resources
    Shader m_depthShader;
    Ref<Framebuffer> m_spotLightShadowFramebuffer;
    Ref<Texture> m_spotLightDepthArray; // GL_TEXTURE_2D_ARRAY (depth)

    // Point light resources
    Shader m_depthShaderCube;
    Ref<Framebuffer> m_pointLightShadowFramebuffer;
    Ref<Texture> m_pointLightDepthCubeArray; // GL_TEXTURE_CUBE_MAP_ARRAY (depth)

    // Directional light resources
    Ref<Framebuffer> m_directionalLightShadowFramebuffer;
    Ref<Texture> m_directionalLightDepthArray; // GL_TEXTURE_2D_ARRAY (depth)

    // State tracking
    int m_spotShadowRes = 1024;
    int m_pointShadowRes = 1024;
    int m_directionalShadowRes = 1024;
};

} // namespace Engine
