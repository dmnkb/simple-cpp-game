#pragma once

#include "Camera.h"
#include "Light.h"
#include "SceneNode.h"
#include "pch.h"

class LightSceneNode : public SceneNode
{
  public:
    LightSceneNode(const glm::vec3& position, const glm::vec3& color);
    LightSceneNode(const glm::vec3& position, const glm::vec3& color, const glm::vec3& rotation);

    const Light prepareLight();
    const Ref<Camera> createShadowCamera();
    const glm::mat4 getTransform();

  private:
    glm::vec3 m_color = {0.0f, 0.0f, 0.0f};
    ELightType m_lightType = ELightType::POINT;
    float m_innerCone = 0.0f;
    float m_outerCone = 0.0f;
};