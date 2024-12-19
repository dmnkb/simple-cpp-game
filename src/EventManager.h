#pragma once

#include "Event.h"
#include "pch.h"

// TODO: Should be static to enforce implicit singleton pattern
class EventManager
{
  public:
    using EventCallback = std::function<void(Event*)>;

    void registerListeners(const std::string& eventType, EventCallback callback);

    void queueEvent(Event* event);

    void processEvents();

  private:
    void dispatchEvent(Event* event);

    std::unordered_map<std::string, std::vector<EventCallback>> listeners;
    std::vector<Event*> eventQueue;
};