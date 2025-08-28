#pragma once

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

} // namespace Engine