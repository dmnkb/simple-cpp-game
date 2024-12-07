#include "Player.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEBUG_MOVEMENT true

Player::Player(Camera& camera, EventManager& eventManager) : m_Camera(camera), m_camChange(0, 0)
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
    float mouseSpeed = 0.3f;

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

// Movement
#if DEBUG_MOVEMENT
    const float speed = 20.0f;
#else
    const float speed = 5.0f;
#endif
    const float jumpForce = 8.0f;
    const float gravity = -9.8f;

    auto movementVector = glm::vec3(0.0f);

    if (isKeyPressed(GLFW_KEY_W))
    {
        movementVector += speed * m_Direction;
    }
    if (isKeyPressed(GLFW_KEY_S))
    {
        movementVector -= speed * m_Direction;
    }
    if (isKeyPressed(GLFW_KEY_A))
    {
        glm::vec3 leftDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
        movementVector += speed * leftDirection;
    }
    if (isKeyPressed(GLFW_KEY_D))
    {
        glm::vec3 rightDirection = glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        movementVector += speed * rightDirection;
    }

    // Jumping
    if (isKeyPressed(GLFW_KEY_SPACE) && m_onGround)
    {
        m_verticalVelocity = jumpForce;
        m_onGround = false;
    }

#if !DEBUG_MOVEMENT
    // Gravity
    if (!m_onGround)
        m_verticalVelocity += gravity * deltaTime;
    else
        m_verticalVelocity = 0.0f;
#endif

    // Collision Detection
    m_collisionPairCheckCount = 0;
    const float height = 1.8f;
    const float halfWidth = 0.3f;
    m_boundingBox = {glm::vec3(m_Position.x - halfWidth, m_Position.y, m_Position.z - halfWidth),
                     glm::vec3(m_Position.x + halfWidth, m_Position.y + height, m_Position.z + halfWidth)};

    m_onGround = false; // Reset ground state

    // Expand collision checking to neighbouring cells
    //
    //    [x]
    // [x][x][x]
    //    [x]
    //
    glm::vec2 collisionCellMap[5] = {{0, 0}, {-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    auto cellSize = level.getCellSize();
    for (int i = 0; i < 5; i++)
    {
        auto cubesInCell = level.getCubesInCell(glm::vec2(m_Position.x, m_Position.z) +
                                                collisionCellMap[i] * (static_cast<float>(cellSize) / 2.f));
        for (const auto& cube : cubesInCell)
        {
            m_collisionPairCheckCount++;
            BoundingBox cubeAABB = {glm::vec3(cube.x - 0.5f, -0.5f, cube.y - 0.5f),
                                    glm::vec3(cube.x + 0.5f, 0.5f, cube.y + 0.5f)};

            if (checkCollision(m_boundingBox, cubeAABB))
            {
                auto collisionSide = getCollisionSide(m_boundingBox, cubeAABB);

                // Check if player is standing on the ground
                if (collisionSide.y > 0.0f && m_verticalVelocity < 0)
                {
                    m_onGround = true;
                    m_Position.y = cubeAABB.max.y; // Snap player to the top of the cube
                    m_verticalVelocity = 0;
                }

                // Project movement vector onto the collision plane
                glm::vec3 collisionNormal = collisionSide; // Use the collision side as the normal
                float dotProduct = glm::dot(movementVector, collisionNormal);

                if (dotProduct < 0.0f) // Only resolve if moving into the collision plane
                {
                    movementVector -= dotProduct * collisionNormal;
                }

                // Prevent further checks since collision has been handled
                break;
            }
        }
    };

    // Final Position Update
    movementVector *= deltaTime;
#if DEBUG_MOVEMENT
    m_Position += movementVector;
#else
    m_Position += movementVector * glm::vec3({1.0f, 0.0f, 1.0f});
#endif
    m_Position.y += m_verticalVelocity * deltaTime;

    // Hack to make the player stop at Y: -1
    if (m_Position.y <= -.5f)
    {
        m_onGround = true;
        m_Position = glm::vec3({m_Position.x, -.5f, m_Position.z});
    }

    // Camera Update
    glm::vec3 adjustedCameraPosition = m_Position + glm::vec3(0.0f, height, 0.0f);
    m_Camera.setPosition(adjustedCameraPosition);
    m_Camera.lookAt(adjustedCameraPosition + m_Direction);
}

bool Player::isKeyPressed(unsigned int key)
{
    auto it = std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key);
    return it != m_PressedKeys.end();
}
