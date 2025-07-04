#include "LightSceneNode.h"
#include "Texture.h"
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
    // TODO: maybe get rid of SceneNode polymorphism?
    m_position = position;
    m_direction = rotation;

    m_shadowCam = CreateRef<Camera>(CameraProps{.aspect = static_cast<float>(m_outerCone * 3 * (M_PI / 180.0f)),
                                                .near = 0.1f,
                                                .far = 1000.0f,
                                                .position = m_position,
                                                .target = position + rotation,
                                                .type = m_lightType == ELT_SPOT ? ECT_PROJECTION : ECT_ORTHOGRAPHIC});

    // Depth buffer
    // clang-format off
    m_shadowFramebuffer = CreateRef<Framebuffer>();
    m_shadowDepthTexture = CreateRef<Texture>(
        TextureProperties{
            .internalFormat = GL_DEPTH_COMPONENT,
            .width = 4096,
            .height = 4096,
            .format = GL_DEPTH_COMPONENT,
            .type = GL_FLOAT,
        },
        CustomProperties{
            .attachmentType = GL_DEPTH_ATTACHMENT,
        });
    m_shadowFramebuffer->attachTexture(m_shadowDepthTexture);
    
    // Debug depth buffer
    m_shadowDebugColorTexture = CreateRef<Texture>(
        TextureProperties{
            .internalFormat = GL_RGB,
            .width = 4096,
            .height = 4096,
            .format = GL_RGB,
            .type = GL_UNSIGNED_BYTE,
        },
        CustomProperties{
            .attachmentType = GL_COLOR_ATTACHMENT0,
        });
    m_shadowFramebuffer->attachTexture(m_shadowDebugColorTexture);
    // clang-format on
}

void LightSceneNode::setLookAt(const glm::vec3& lookAt)
{
    glm::vec3 direction = glm::normalize(lookAt - m_position);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::quat rotationQuat = glm::rotation(forward, direction);
    glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat);
    m_rotation = glm::degrees(eulerAngles);

    m_shadowCam->lookAt(lookAt);
}
