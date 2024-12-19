#pragma once

#include "Event.h"
#include "pch.h"

using EventCallback = std::function<void(Event*)>;
struct EventData
{
    std::unordered_map<std::string, std::vector<EventCallback>> listeners;
    std::vector<Event*> eventQueue;
};

class EventManager
{
  public:
    static void registerListeners(const std::string& eventType, EventCallback callback);
    static void queueEvent(Event* event);
    static void processEvents();

  private:
    static void dispatchEvent(Event* event);
};