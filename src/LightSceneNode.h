#pragma once

#include "Camera.h"
#include "Framebuffer.h"
#include "SceneNode.h"
#include "TextureManager.h"
#include "pch.h"

enum ELightType
{
    ELT_POINT,
    ELT_DIRECTIONAL,
    ELT_SPOT
};

class LightSceneNode : public SceneNode
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

    ELightType getLightType() const
    {
        return m_lightType;
    }

    Ref<Camera> getShadowCam()
    {
        return m_shadowCam;
    }

    Ref<Framebuffer> getFrameBuffer()
    {
        return m_frameBuffer;
    }

    Ref<Texture> getDepthBuffer()
    {
        return m_depthBuffer;
    }

    // Converts this light to its GPU-ready format
    LightUBO toUBO() const
    {
        return {m_position, 0.0f, m_color, 0.0f, m_direction, 0.0f, m_lightType, m_innerCone, m_outerCone, 0.0f};
    }

  private:
    // Common attributes
    ELightType m_lightType = ELightType::ELT_POINT;
    glm::vec3 m_color = {0.0f, 0.0f, 0.0f};

    // Spot lights only
    glm::vec3 m_direction = {0.0f, 0.0f, 0.0f};
    float m_innerCone = 0.0f;
    float m_outerCone = 0.0f;

    // Shadow mapping
    Ref<Camera> m_shadowCam;
    Ref<Texture> m_depthBuffer;
    Ref<Framebuffer> m_frameBuffer;
};