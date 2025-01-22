#pragma once

#include "Camera.h"
#include "Light.h"
#include "SceneNode.h"
#include "pch.h"

class LightSceneNode : public SceneNode
{
  public:
    LightSceneNode(const glm::vec3& position, const glm::vec3& color = {1.f, 1.f, 1.f},
                   const glm::vec3& rotation = {0, 0, 0}, const ELightType& lightType = ELT_POINT,
                   const float& innerCone = 20.f, const float& outerCone = 30.f);

    void setLookAt(const glm::vec3& lookAt = {0.f, 0.f, 0.f});

    const Light prepareLight();
    const Ref<Camera> createShadowCamera();

  private:
    glm::vec3 m_color = {0.0f, 0.0f, 0.0f};
    ELightType m_lightType = ELightType::ELT_POINT;
    float m_innerCone = 0.0f;
    float m_outerCone = 0.0f;
};