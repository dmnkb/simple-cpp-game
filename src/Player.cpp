#include "Player.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

Player::Player(Camera& camera, EventManager& eventManager)
    : m_Camera(camera), m_Position(0, 0, 5), m_Rotation(-180, 0), m_camChange(0, 0)
{
    eventManager.registerListeners(typeid(KeyEvent).name(), [this](Event* event) { this->onKeyEvent(event); });
    eventManager.registerListeners(typeid(MouseMoveEvent).name(),
                                   [this](Event* event) { this->onMouseMoveEvent(event); });
}

void Player::onKeyEvent(Event* event)
{
    auto keyEvent = dynamic_cast<KeyEvent*>(event);
    if (!keyEvent)
        return;

    if (keyEvent->action > GLFW_RELEASE)
    {
        if (std::find(m_PressedKeys.begin(), m_PressedKeys.end(), keyEvent->key) == m_PressedKeys.end())
        {
            m_PressedKeys.push_back(keyEvent->key);
        }
    }
    else
    {
        auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), keyEvent->key);
        if (it != m_PressedKeys.end())
        {
            m_PressedKeys.erase(it);
        }
    }
}

void Player::onMouseMoveEvent(Event* event)
{
    if (auto mouseMoveEvent = dynamic_cast<MouseMoveEvent*>(event))
    {
        m_camChange.x = mouseMoveEvent->speedX;
        m_camChange.y = mouseMoveEvent->speedY;
    }
}

void Player::update(double deltaTime)
{
    // Rotation
    float mouseSpeed = .3;

    m_Rotation.x += m_camChange.x * mouseSpeed;
    m_Rotation.y += m_camChange.y * mouseSpeed;
    m_Rotation.y = std::clamp(m_Rotation.y, -89.0f, 89.0f);
    m_camChange = glm::vec2(0.0f);

    float yawRadians = glm::radians(m_Rotation.x);
    float pitchRadians = glm::radians(m_Rotation.y);

    m_Direction.x = cos(pitchRadians) * sin(yawRadians);
    m_Direction.y = sin(pitchRadians);
    m_Direction.z = cos(pitchRadians) * cos(yawRadians);
    m_Direction = glm::normalize(m_Direction);

    // Position
    float speed = 5.0f * deltaTime;

    if (isKeyPressed(GLFW_KEY_W))
    {
        m_Position += speed * m_Direction;
    }
    if (isKeyPressed(GLFW_KEY_S))
    {
        m_Position -= speed * m_Direction;
    }
    if (isKeyPressed(GLFW_KEY_A))
    {
        glm::vec3 leftDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
        m_Position += speed * leftDirection;
    }
    if (isKeyPressed(GLFW_KEY_D))
    {
        glm::vec3 rightDirection = glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_Position += speed * rightDirection;
    }

    // Update both
    m_Camera.setPosition(m_Position);
    m_Camera.lookAt(m_Position + m_Direction);
}

bool Player::isKeyPressed(unsigned int key)
{
    auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key);
    return it != m_PressedKeys.end();
}
