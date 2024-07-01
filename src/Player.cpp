#include "Player.h"
#include <GLFW/glfw3.h>
#include <string>

Player::Player(Camera& camera, EventManager& eventManager) : m_Camera(camera)
{
    eventManager.registerListeners(typeid(KeyEvent).name(),
                                   [this](std::shared_ptr<Event> event) { this->onKeyEvent(event); });
    eventManager.registerListeners(typeid(MousePosEvent).name(),
                                   [this](std::shared_ptr<Event> event) { this->onMousePosEvent(event); });
}

void Player::onKeyEvent(std::shared_ptr<Event> event)
{
    auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(event);
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
            m_PressedKeys.erase(it); // Erase using the iterator returned by std::find
        }
    }
}

void Player::onMousePosEvent(std::shared_ptr<Event> event)
{
    if (auto mousePosEvent = std::dynamic_pointer_cast<MousePosEvent>(event))
        printf("x: %f, y: %f\n", mousePosEvent->xpos, mousePosEvent->ypos);
}

void Player::update()
{
    m_Camera.setPosition(m_Position[0], m_Position[1], m_Position[2]);
    if (isKeyPressed(GLFW_KEY_W))
        m_Position[2] += .01;
    if (isKeyPressed(GLFW_KEY_S))
        m_Position[2] -= .01;
}

bool Player::isKeyPressed(unsigned int key)
{
    auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key);
    return it != m_PressedKeys.end();
}
