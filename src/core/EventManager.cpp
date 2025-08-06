#include "EventManager.h"
#include "pch.h"

namespace Engine
{

static EventData s_eventData;

// TODO: ENUM
void EventManager::registerListeners(const std::string& eventType, EventCallback callback)
{
    s_eventData.listeners[eventType].push_back(callback);
}

void EventManager::queueEvent(const Ref<Event> event)
{
    s_eventData.eventQueue.push_back(event);
}

void EventManager::processEvents()
{
    for (auto event : s_eventData.eventQueue)
    {
        dispatchEvent(event);
    }
    s_eventData.eventQueue.clear();
}

void EventManager::dispatchEvent(Ref<Event> event)
{
    auto eventType = typeid(*event).name();
    if (s_eventData.listeners.find(eventType) != s_eventData.listeners.end())
    {
        for (auto& listener : s_eventData.listeners[eventType])
        {
            listener(event);
        }
    }
}

} // namespace Engine