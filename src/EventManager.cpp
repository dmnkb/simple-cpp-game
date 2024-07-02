#include "EventManager.h"
#include <string>

void EventManager::registerListeners(const std::string& eventType, EventCallback callback)
{
    listeners[eventType].push_back(callback);
}

void EventManager::queueEvent(Event* event)
{
    eventQueue.push_back(event);
}

void EventManager::processEvents()
{
    for (auto event : eventQueue)
    {
        dispatchEvent(event);
    }
    for (auto event : eventQueue)
    {
        delete event;
    }
    eventQueue.clear();
}

void EventManager::dispatchEvent(Event* event)
{
    auto eventType = typeid(*event).name();
    if (listeners.find(eventType) != listeners.end())
    {
        for (auto& listener : listeners[eventType])
        {
            listener(event);
        }
    }
}