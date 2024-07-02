#include "EventManager.h"
#include <string>

void EventManager::registerListeners(const std::string& eventType, EventCallback callback)
{
    printf("Registering listener for event type: %s\n", eventType.c_str());

    listeners[eventType].push_back(callback);
}

void EventManager::queueEvent(Event* event)
{
    printf("queueEvent: %s\n", typeid(*event).name());
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
    printf("dispatchEvent %s \n", eventType);
    if (listeners.find(eventType) != listeners.end())
    {
        for (auto& listener : listeners[eventType])
        {
            listener(event);
        }
    }
}