#include "Player.h"
#include <GLFW/glfw3.h>
#include <string>

Player::Player(Camera& camera, EventManager& eventManager) : m_Camera(camera), m_Position{0, 0, 2}, m_Rotation{0, 0}
{
    eventManager.registerListeners(typeid(KeyEvent).name(), [this](Event* event) { this->onKeyEvent(event); });
    eventManager.registerListeners(typeid(MousePosEvent).name(),
                                   [this](Event* event) { this->onMousePosEvent(event); });
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

void Player::onMousePosEvent(Event* event)
{
    if (auto mousePosEvent = dynamic_cast<MousePosEvent*>(event))
    {
        m_camChange.x = m_cursorPositionOld.x - mousePosEvent->xpos;
        m_camChange.y = m_cursorPositionOld.y - mousePosEvent->ypos;
        m_cursorPositionOld.x = mousePosEvent->xpos;
        m_cursorPositionOld.y = mousePosEvent->ypos;
    }
}

void Player::update()
{
    // Rotation
    m_Rotation[0] += m_camChange.x;
    m_Rotation[1] += m_camChange.y;

    m_Rotation[1] = std::max(-89.0f, std::min(89.0f, m_Rotation[1]));

    float yawRadians = m_Rotation[0] * (M_PI / 180.0);
    float pitchRadians = m_Rotation[1] * (M_PI / 180.0);

    m_Direction[0] = cos(pitchRadians) * sin(yawRadians);
    m_Direction[1] = sin(pitchRadians);
    m_Direction[2] = cos(pitchRadians) * cos(yawRadians);

    float length =
        sqrt(m_Direction[0] * m_Direction[0] + m_Direction[1] * m_Direction[1] + m_Direction[2] * m_Direction[2]);
    m_Direction[0] /= length;
    m_Direction[1] /= length;
    m_Direction[2] /= length;

    m_Camera.lookAt(m_Position[0] + m_Direction[0], m_Position[1] + m_Direction[1], m_Position[2] + m_Direction[2]);

    m_camChange.x = 0;
    m_camChange.y = 0;

    // Position
    float speed = 0.01f;

    if (isKeyPressed(GLFW_KEY_W))
    {
        m_Position[0] += speed * m_Direction[0];
        m_Position[1] += speed * m_Direction[1];
        m_Position[2] += speed * m_Direction[2];
    }
    if (isKeyPressed(GLFW_KEY_S))
    {
        m_Position[0] -= speed * m_Direction[0];
        m_Position[1] -= speed * m_Direction[1];
        m_Position[2] -= speed * m_Direction[2];
    }
    if (isKeyPressed(GLFW_KEY_A))
    {
        float leftRadians = (m_Rotation[0] + 90.0f) * (M_PI / 180.0);
        float leftDirectionX = sin(leftRadians);
        float leftDirectionZ = cos(leftRadians);
        m_Position[0] += speed * leftDirectionX;
        m_Position[2] += speed * leftDirectionZ;
    }
    if (isKeyPressed(GLFW_KEY_D))
    {
        float rightRadians = (m_Rotation[0] - 90.0f) * (M_PI / 180.0);
        float rightDirectionX = sin(rightRadians);
        float rightDirectionZ = cos(rightRadians);
        m_Position[0] += speed * rightDirectionX;
        m_Position[2] += speed * rightDirectionZ;
    }

    m_Camera.setPosition(m_Position[0], m_Position[1], m_Position[2]);
}

bool Player::isKeyPressed(unsigned int key)
{
    auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key);
    return it != m_PressedKeys.end();
}
