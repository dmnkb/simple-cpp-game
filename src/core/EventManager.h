#pragma once

#include "Core.h"
#include "Event.h"
#include "pch.h"

namespace Engine
{

using EventCallback = std::function<void(const Ref<Event>)>;

struct EventHandle
{
    std::string eventType;
    std::uint64_t id = 0;
    explicit operator bool() const
    {
        return id != 0;
    }
};

struct EventListener
{
    std::uint64_t id;
    EventCallback cb;
};

struct EventData
{
    std::unordered_map<std::string, std::vector<EventListener>> listeners;
    std::vector<Ref<Event>> eventQueue;
    std::uint64_t nextId = 1;
};

class EventManager
{
  public:
    static EventHandle registerListeners(const std::string& eventType, EventCallback callback);
    static void unregister(const EventHandle& handle);

    static void queueEvent(const Ref<Event> event);
    static void processEvents();

  private:
    static void dispatchEvent(const Ref<Event> event);
};

} // namespace Engine