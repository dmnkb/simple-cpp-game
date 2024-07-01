#include "Player.h"
#include <GLFW/glfw3.h>
#include <string>

Player::Player(Camera& camera, EventManager& eventManager) : m_Camera(camera), m_KeyW(false), m_KeyS(false)
{
    eventManager.registerListeners(typeid(KeyEvent).name(),
                                   [this](std::shared_ptr<Event> event) { this->onKeyEvent(event); });
    eventManager.registerListeners(typeid(MousePosEvent).name(),
                                   [this](std::shared_ptr<Event> event) { this->onMousePosEvent(event); });
}

void Player::onKeyEvent(std::shared_ptr<Event> event)
{
    if (auto keyEvent = std::dynamic_pointer_cast<KeyEvent>(event))
    {
        if (keyEvent->key == GLFW_KEY_W)
            m_KeyW = keyEvent->action > GLFW_RELEASE;

        if (keyEvent->key == GLFW_KEY_S)
            m_KeyS = keyEvent->action > GLFW_RELEASE;
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
    if (m_KeyW)
        m_Position[2] += .01;
    if (m_KeyS)
        m_Position[2] -= .01;
}
