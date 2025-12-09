#pragma once

#include <glm/glm.hpp>

namespace Engine
{

class Event
{
  public:
    virtual ~Event() = default;
};

class KeyEvent : public Event
{
  public:
    KeyEvent(int key, int action) : key(key), action(action) {}
    int key;
    int action;
};

class MouseMoveEvent : public Event
{
  public:
    MouseMoveEvent(float speedX, float speedY, bool cursorDisabled)
        : speedX(speedX), speedY(speedY), cursorDisabled(cursorDisabled)
    {
    }
    float speedX = 0.0f;
    float speedY = 0.0f;
    bool cursorDisabled = false;
};

class WindowReziseEvent : public Event
{
  public:
    WindowReziseEvent(int windowWidth, int windowHeight) : windowWidth(windowWidth), windowHeight(windowHeight) {}
    int windowWidth;
    int windowHeight;
};

class MainCameraChangedEvent : public Event
{
  public:
    MainCameraChangedEvent(glm::vec3 pos, glm::vec3 fwd, glm::vec3 up_, float fovy, float aspect_, float zn, float zf)
        : position(pos), forward(glm::normalize(fwd)), up(glm::normalize(up_)), fovY(fovy), aspect(aspect_), znear(zn),
          zfar(zf)
    {
    }

    glm::vec3 position{};
    glm::vec3 forward{0, 0, -1};
    glm::vec3 up{0, 1, 0};
    float fovY{glm::radians(60.f)};
    float aspect{16.f / 9.f};
    float znear{0.1f};
    float zfar{1000.f};
};

} // namespace Engine