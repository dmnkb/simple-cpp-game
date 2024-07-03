#pragma once

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
    MouseMoveEvent(float speedX, float speedY) : speedX(speedX), speedY(speedY) {}
    float speedX, speedY;
};