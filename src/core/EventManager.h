#pragma once

#include "Core.h"
#include "Event.h"
#include "pch.h"

namespace Engine
{

using EventCallback = std::function<void(const Ref<Event>)>;
struct EventData
{
    std::unordered_map<std::string, std::vector<EventCallback>> listeners;
    std::vector<Ref<Event>> eventQueue;
};

class EventManager
{
  public:
    static void registerListeners(const std::string& eventType, EventCallback callback);
    static void queueEvent(const Ref<Event> event);
    static void processEvents();

  private:
    static void dispatchEvent(const Ref<Event> event);
};

} // namespace Engine