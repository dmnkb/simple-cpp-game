#include "LightSceneNode.h"
#include "Light.h"
#include "pch.h"
#include <glm/glm.hpp>

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