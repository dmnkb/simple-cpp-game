#pragma once

#include "Framebuffer.h"
#include "Material.h"
#include "PassIO.h"
#include "SpotLight.h"
#include "scene/Scene.h"

namespace Engine
{

#define MAX_SPOT_LIGHTS 16
#define MAX_POINT_LIGHTS 16

class LightingPass
{
  public:
    struct alignas(16) SpotLightsUBO
    {
        glm::vec4 positionsWS[MAX_SPOT_LIGHTS];
        glm::vec4 directionsWS[MAX_SPOT_LIGHTS];
        glm::vec4 colorsIntensity[MAX_SPOT_LIGHTS];
        glm::vec4 conesRange[MAX_SPOT_LIGHTS];
        glm::vec4 attenuations[MAX_SPOT_LIGHTS];
    };
    static_assert(alignof(SpotLightsUBO) == 16, "std140 alignment");

    struct alignas(16) PointLightsUBO
    {
        glm::vec4 positionsWS[MAX_POINT_LIGHTS];
        glm::vec4 colorsIntensity[MAX_POINT_LIGHTS];
        glm::vec4 ranges[MAX_POINT_LIGHTS];
        glm::vec4 attenuations[MAX_POINT_LIGHTS];
    };
    static_assert(alignof(PointLightsUBO) == 16, "std140 alignment");

    LightingPass();
    ~LightingPass();

    void execute(Scene& scene, const LightingInputs& litIn);
    Ref<Texture> getRenderTargetTexture()
    {
        return m_renderTargetTexture;
    }

  private:
    void uploadUniforms(Scene& scene, const Ref<Material>& material, const LightingInputs& litIn);

  private:
    SpotLightsUBO m_spotLightsCPU{}; // CPU staging buffer
    GLuint m_spotLightsUBO = 0;      // GL UBO handle (binding = 0)

    Ref<Framebuffer> m_frameBuffer;
    Ref<Texture> m_renderTargetTexture;
};

} // namespace Engine