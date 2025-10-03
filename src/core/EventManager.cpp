#include "EventManager.h"
#include "pch.h"

namespace Engine
{

static EventData s_eventData;

EventHandle EventManager::registerListeners(const std::string& eventType, EventCallback callback)
{
    const std::uint64_t id = s_eventData.nextId++;
    s_eventData.listeners[eventType].push_back(EventListener{id, std::move(callback)});
    return EventHandle{eventType, id};
}

void EventManager::unregister(const EventHandle& handle)
{
    if (!handle)
        return;

    auto it = s_eventData.listeners.find(handle.eventType);
    if (it == s_eventData.listeners.end())
        return;

    auto& vec = it->second;

    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const EventListener& L) { return L.id == handle.id; }),
              vec.end());

    if (vec.empty())
    {
        s_eventData.listeners.erase(it);
    }
}

void EventManager::queueEvent(const Ref<Event> event)
{
    s_eventData.eventQueue.push_back(event);
}

void EventManager::processEvents()
{
    for (auto& event : s_eventData.eventQueue)
        dispatchEvent(event);

    s_eventData.eventQueue.clear();
}

void EventManager::dispatchEvent(const Ref<Event> event)
{
    auto eventType = typeid(*event).name();
    if (s_eventData.listeners.find(eventType) != s_eventData.listeners.end())
    {
        for (auto& listener : s_eventData.listeners[eventType])
        {
            listener.cb(event);
        }
    }
}

} // namespace Engine