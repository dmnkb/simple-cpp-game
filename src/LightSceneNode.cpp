#include "LightSceneNode.h"
#include "Light.h"
#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

LightSceneNode::LightSceneNode(const glm::vec3& position, const glm::vec3& color, const glm::vec3& rotation,
                               const ELightType& lightType, const float& innerCone, const float& outerCone)
    : SceneNode(), m_color(color), m_lightType(lightType), m_innerCone(innerCone), m_outerCone(outerCone)
{
    m_position = position;
    m_rotation = rotation;
}

void LightSceneNode::setLookAt(const glm::vec3& lookAt)
{
    glm::vec3 direction = glm::normalize(lookAt - m_position);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::quat rotationQuat = glm::rotation(forward, direction);
    glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);
    m_rotation = glm::degrees(eulerAngles);
}

const ELightType LightSceneNode::getLightType()
{
    return m_lightType;
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
        m_innerCone, // Inner cone angle in radians
        m_outerCone, // Outer cone angle in radians
        0.0f,        // Padding
    };
}

const Ref<Camera> LightSceneNode::createShadowCamera()
{
    CameraProps shadowCamProps = {.aspect = static_cast<float>(60.0f * (M_PI / 180.0f)),
                                  .aspect = 1.0f,
                                  .near = 0.1f,
                                  .far = 1000.0f,
                                  .position = m_position,
                                  .target = m_position + m_rotation,
                                  .type = m_lightType == ELT_SPOT ? ECT_PROJECTION : ECT_ORTHOGRAPHIC};
    auto camera = CreateRef<Camera>(shadowCamProps);
    return camera;
}