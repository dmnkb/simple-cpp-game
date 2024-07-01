#pragma once

#include "Event.h"
#include <functional>
#include <string>

class EventManager
{
  public:
    using EventCallback = std::function<void(std::shared_ptr<Event>)>;

    void registerListeners(const std::string& eventType, EventCallback callback);

    void queueEvent(std::shared_ptr<Event> event);

    void processEvents();

  private:
    void dispatchEvent(std::shared_ptr<Event> event);

    std::unordered_map<std::string, std::vector<EventCallback>> listeners;
    std::vector<std::shared_ptr<Event>> eventQueue;
};