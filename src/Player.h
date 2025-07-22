#pragma once

#include "Camera.h"
#include "EventManager.h"
#include "Scene.h"
#include "pch.h"
#include <glm/glm.hpp>

class Player
{
  public:
    Player();

    void onKeyEvent(const Ref<Event> event);
    void onMouseMoveEvent(const Ref<Event> event);
    void update(const Scene& scene, double deltaTime);
    bool isKeyPressed(unsigned int key);

    glm::vec3 getPosition()
    {
        return m_Position;
    }
    glm::vec2 getRotation()
    {
        return m_Rotation;
    }

  private:
    // Core player attributes
    glm::vec3 m_Position = glm::vec3({-50, 30, -50});
    glm::vec2 m_Rotation = glm::vec2({45, -15});
    glm::vec3 m_Direction = glm::vec3(0.f);
    float m_verticalVelocity = 0;

    // Camera
    glm::vec2 m_camChange = glm::vec2(0, 0);
    glm::vec2 m_cursorPositionOld;

    // TODO: Hashset
    // Input handling
    std::vector<unsigned int> m_PressedKeys;

    // Performance
    int m_collisionPairCheckCount = 0;
};