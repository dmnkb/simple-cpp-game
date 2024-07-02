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
        printf("x: %f, y: %f\n", mousePosEvent->xpos, mousePosEvent->ypos);
}

void Player::update()
{
    m_Camera.setPosition(m_Position[0], m_Position[1], m_Position[2]);
    if (isKeyPressed(GLFW_KEY_W))
        m_Position[2] -= .01;
    if (isKeyPressed(GLFW_KEY_S))
        m_Position[2] += .01;
    if (isKeyPressed(GLFW_KEY_A))
        m_Position[0] -= .01;
    if (isKeyPressed(GLFW_KEY_D))
        m_Position[0] += .01;
}

bool Player::isKeyPressed(unsigned int key)
{
    auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key);
    return it != m_PressedKeys.end();
}
