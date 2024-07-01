#include "EventManager.h"
#include <string>

void EventManager::registerListeners(const std::string& eventType, EventCallback callback)
{
    printf("Registering listener for event type: %s\n", eventType.c_str());

    listeners[eventType].push_back(callback);
}

void EventManager::queueEvent(std::shared_ptr<Event> event)
{
    eventQueue.push_back(event);
}

void EventManager::processEvents()
{
    for (auto& event : eventQueue)
    {
        dispatchEvent(event);
    }
    eventQueue.clear();
}

void EventManager::dispatchEvent(std::shared_ptr<Event> event)
{
    std::string eventType = typeid(*event).name();
    if (listeners.find(eventType) != listeners.end())
    {
        for (auto& listener : listeners[eventType])
        {
            listener(event);
        }
    }
}