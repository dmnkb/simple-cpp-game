#include "Player.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Player::Player(Camera& camera, EventManager& eventManager)
    : m_Camera(camera), m_Position(0, 20, 0), m_Rotation(45, -20), m_camChange(0, 0)
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
    if (m_IsCursorDisabled)
        return;

    if (auto mouseMoveEvent = dynamic_cast<MouseMoveEvent*>(event))
    {
        m_camChange.x = mouseMoveEvent->speedX;
        m_camChange.y = mouseMoveEvent->speedY;
    }
}

void Player::update(double deltaTime, Level& level)
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
    float speed = 10.0f * deltaTime;
    glm::vec3 newPosition = m_Position;

    if (isKeyPressed(GLFW_KEY_W))
    {
        newPosition += speed * m_Direction;
    }
    if (isKeyPressed(GLFW_KEY_S))
    {
        newPosition -= speed * m_Direction;
    }
    if (isKeyPressed(GLFW_KEY_A))
    {
        glm::vec3 leftDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
        newPosition += speed * leftDirection;
    }
    if (isKeyPressed(GLFW_KEY_D))
    {
        glm::vec3 rightDirection = glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        newPosition += speed * rightDirection;
    }

    // Collision detection
    const float height = 1.8f;
    const float halfWidth = .3f;
    m_boundingBox = {glm::vec3(newPosition - halfWidth), glm::vec3(newPosition + halfWidth)};

    bool collides = false;

    std::vector<glm::vec2> cubesWithinRadius = level.getCubesInsideRadius(newPosition, 3);
    for (auto& cube : cubesWithinRadius)
    {
        BoundingBox cubeAABB = {glm::vec3(cube.x - .5, -.5, cube.y - .5), glm::vec3(cube.x + .5, .5, cube.y + .5)};
        if (checkCollision(m_boundingBox, cubeAABB))
        {
            collides = true;
            break;
        }
    }

    // Update new position
    if (!collides)
    {
        m_Position = newPosition;
    }

    // Update camera
    m_Camera.setPosition(m_Position);
    m_Camera.lookAt(m_Position + m_Direction);
}

bool Player::isKeyPressed(unsigned int key)
{
    auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key);
    return it != m_PressedKeys.end();
}
