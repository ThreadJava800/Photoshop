#include "events.h"

EventProcessable::EventProcessable() :
    priority(0)         {}

EventProcessable::EventProcessable(uint8_t _priority) :
    priority(_priority) {}

uint8_t EventProcessable::getPriority() {
    return priority;
}

EventManager::EventManager() :
    EventProcessable(),
    mainWindowPtr   (nullptr)  {
        resetPriorities();
        children = new List<EventProcessable*>();
    }

EventManager::EventManager(Window* _mainWindowPtr) :
    EventProcessable(),
    mainWindowPtr   (_mainWindowPtr)    {
        resetPriorities();
        children = new List<EventProcessable*>();
    }

EventManager::~EventManager() {
    resetPriorities();
    delete children;
}

void EventManager::registerObject  (EventProcessable* eventProc) {

}

void EventManager::unregisterObject(EventProcessable* eventProc) {

}

void EventManager::privatizeEvents (List<EventType> events, int priority) {

}

void EventManager::resetPriorities () {
    
}