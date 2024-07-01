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

class MousePosEvent : public Event
{
  public:
    MousePosEvent(double xpos, double ypos) : xpos(xpos), ypos(ypos) {}
    double xpos, ypos;
};