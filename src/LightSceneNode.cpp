#include "LightSceneNode.h"
#include "Light.h"
#include "pch.h"
#include <glm/ext.hpp>

LightSceneNode::LightSceneNode(const glm::vec3& position, const glm::vec3& color)
    : SceneNode(), m_color(color), m_lightType(ELightType::POINT)
{
    m_position = position;
}

LightSceneNode::LightSceneNode(const glm::vec3& position, const glm::vec3& color, const glm::vec3& rotation)
    : SceneNode(), m_color(color), m_lightType(ELightType::SPOT)
{
    m_position = position;
    m_rotation = rotation;
}

const Light LightSceneNode::prepareLight()
{
    return {
        m_position,  // Position
        0.0f,        // Padding
        m_color,     // Color: red
        0.0f,        // Padding
        m_rotation,  // Direction: SPOTing downward
        0.0f,        // Padding
        m_lightType, // ELightType,
        20.0f,       // Inner cone angle in radians
        30.0f,       // Outer cone angle in radians
        0.0f,        // Padding
    };
}

const Ref<Camera> LightSceneNode::createShadowCamera()
{
    // TODO: The cone is currently hardcoded
    CameraProps shadowCamProps = {static_cast<float>(30.0f * (M_PI / 180.0f)), 1, 0.1f, 1000.0f};
    auto camera = CreateRef<Camera>(shadowCamProps);
    camera->setPosition(m_position);
    camera->lookAt({m_rotation});
    return camera;
}

const glm::mat4 LightSceneNode::getTransform()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), {1.0f, 0.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), {0.0f, 1.0f, 0.0f}) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), {0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4(1.0f), m_scale);

    return transform;
}