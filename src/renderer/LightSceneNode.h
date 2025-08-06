#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "Texture.h"

namespace Engine
{

enum ELightType
{
    ELT_POINT,
    ELT_DIRECTIONAL,
    ELT_SPOT
};

class LightSceneNode
{
  public:
    struct LightUBO // GPU ready, only contains data for the GPU
    {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};      // 3 floats, 16 bytes due to std140 padding
        float padding1 = 0.0f;                        // Padding to align the next vec3
        glm::vec3 color = {0.0f, 0.0f, 0.0f};         // 3 floats, 16 bytes due to std140 padding
        float padding2 = 0.0f;                        // Padding to align the next vec3
        glm::vec3 rotation = {0.0f, 0.0f, 0.0f};      // 3 floats, 16 bytes due to std140 padding
        float padding3 = 0.0f;                        // Padding to align the next vec3
        ELightType lightType = ELightType::ELT_POINT; //
        float innerCone = 0.0f;                       // 4 bytes
        float outerCone = 0.0f;                       // 4 bytes
        float padding4 = 0.0f;                        // Padding to align the struct to 16 bytes
    };

  public:
    LightSceneNode(const glm::vec3& position, const glm::vec3& color = {1.f, 1.f, 1.f},
                   const glm::vec3& rotation = {0, 0, 0}, const ELightType& lightType = ELT_POINT,
                   const float& innerCone = 20.f, const float& outerCone = 30.f);

    void setLookAt(const glm::vec3& lookAt = {0.f, 0.f, 0.f});

    void setPosition(const glm::vec3& position)
    {
        m_position = position;
        m_shadowCam->setPosition(position);
    }

    ELightType getLightType() const
    {
        return m_lightType;
    }

    Ref<Camera> getShadowCam()
    {
        return m_shadowCam;
    }

    Ref<Framebuffer> getShadowFrameBuffer()
    {
        return m_shadowFramebuffer;
    }

    Ref<Texture> getShadowDepthTexture()
    {
        return m_shadowDepthTexture;
    }

    Ref<Texture> getShadowDebugColorTexture()
    {
        return m_shadowDebugColorTexture;
    }

    // Converts this light to its GPU-ready format
    LightUBO toUBO() const
    {
        // FIXME: Currently there are both m_direction and m_rotation. Get rid of the later one
        return {m_position, 0.0f, m_color, 0.0f, m_direction, 0.0f, m_lightType, m_innerCone, m_outerCone, 0.0f};
    }

  private:
    // Common attributes
    glm::vec3 m_position = glm::vec3{0.0f};
    glm::vec3 m_rotation = glm::vec3{0.0f};
    ELightType m_lightType = ELightType::ELT_POINT;
    glm::vec3 m_color = {0.0f, 0.0f, 0.0f};

    // Spot lights only
    glm::vec3 m_direction = {0.0f, 0.0f, 0.0f};
    float m_innerCone = 0.0f;
    float m_outerCone = 0.0f;

    // Shadow mapping
    Ref<Camera> m_shadowCam;
    Ref<Texture> m_shadowDepthTexture;      // stores depth values from light's POV
    Ref<Texture> m_shadowDebugColorTexture; // color attachment for visualizing during shadow pass
    Ref<Framebuffer> m_shadowFramebuffer;
};

} // namespace Engine