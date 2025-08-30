#include "Player.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <algorithm> // clamp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{

Player::Player()
{
    EventManager::registerListeners(typeid(KeyEvent).name(),
                                    [this](const Ref<Event> event) { this->onKeyEvent(event); });
    EventManager::registerListeners(typeid(MouseMoveEvent).name(),
                                    [this](const Ref<Event> event) { this->onMouseMoveEvent(event); });

    // Ensure target starts at current
    m_TargetPosition = m_Position;
}

void Player::onKeyEvent(const Ref<Event> event)
{
    auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(event);
    if (!keyEvent)
        return;

    if (keyEvent->action > GLFW_RELEASE)
    {
        if (std::find(m_PressedKeys.begin(), m_PressedKeys.end(), keyEvent->key) == m_PressedKeys.end())
            m_PressedKeys.push_back(keyEvent->key);
    }
    else
    {
        auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), keyEvent->key);
        if (it != m_PressedKeys.end())
            m_PressedKeys.erase(it);
    }
}

void Player::onMouseMoveEvent(const Ref<Event> event)
{
    if (auto mouseMoveEvent = std::dynamic_pointer_cast<MouseMoveEvent>(event))
    {
        if (mouseMoveEvent->cursorDisabled)
        {
            m_camChange.x = mouseMoveEvent->speedX;
            m_camChange.y = mouseMoveEvent->speedY;
        }
    }
}

void Player::update(const Scene& scene, double deltaTime)
{
    // Rotation
    float mouseSpeed = 0.3f;

    m_Rotation.x += m_camChange.x * mouseSpeed; // yaw
    m_Rotation.y += m_camChange.y * mouseSpeed; // pitch
    m_Rotation.y = std::clamp(m_Rotation.y, -89.0f, 89.0f);
    m_camChange = glm::vec2(0.0f);

    float yawRadians = glm::radians(m_Rotation.x);
    float pitchRadians = glm::radians(m_Rotation.y);

    m_Direction.x = cos(pitchRadians) * sin(yawRadians);
    m_Direction.y = sin(pitchRadians);
    m_Direction.z = cos(pitchRadians) * cos(yawRadians);
    m_Direction = glm::normalize(m_Direction);

    const float speed = 20.0f;
    const float gravity = -9.8f;

    glm::vec3 movementVector(0.0f);

    if (isKeyPressed(GLFW_KEY_W))
        movementVector += speed * m_Direction;
    if (isKeyPressed(GLFW_KEY_S))
        movementVector -= speed * m_Direction;
    if (isKeyPressed(GLFW_KEY_A))
    {
        glm::vec3 left = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
        movementVector += speed * left;
    }
    if (isKeyPressed(GLFW_KEY_D))
    {
        glm::vec3 right = glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        movementVector += speed * right;
    }

    // Integrate target using deltaTime (gameplay space)
    m_TargetPosition += movementVector * static_cast<float>(deltaTime);

    // Vertical integration (keep your existing logic)
    m_TargetPosition.y += m_verticalVelocity * static_cast<float>(deltaTime);

    // SmoothDamp position toward the
    m_Position = SmoothDamp(m_Position,                   // current
                            m_TargetPosition,             // target
                            m_PositionVel,                // velocity accumulator (kept across frames)
                            0.1f,                         // smooth time (s)
                            static_cast<float>(deltaTime) // DT

    );

    // Camera Update
    scene.getActiveCamera()->setPosition(m_Position);
    scene.getActiveCamera()->lookAt(m_Position + m_Direction);
}

bool Player::isKeyPressed(unsigned int key)
{
    auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key);
    return it != m_PressedKeys.end();
}

} // namespace Engine
