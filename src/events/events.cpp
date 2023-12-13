#include "events.h"

EventProcessable::EventProcessable() :
    priority(0)         {}

EventProcessable::EventProcessable(uint8_t _priority) :
    priority(_priority) {}

uint8_t EventProcessable::getPriority() const {
    return priority;
}

EventManager::EventManager() :
    EventProcessable()  {
        resetPriorities();
        children = new List<EventProcessableI*>();
    }

EventManager::~EventManager() {
    delete children;
}

void EventManager::privatizeEvents(List<EventType>& events, int priority) {
    size_t eventCnt = events.getSize();
    for (size_t i = 0; i < eventCnt; i++) {
        priorities[(int)(events[i])] = priority;
    }
}

void EventManager::resetPriorities() {
    for (size_t i = 0; i < EVENT_TYPES_NUM; i++)
        priorities[i] = 0;
}

void EventManager::registerObject (plugin::EventProcessableI *object) {
    ON_ERROR(!object, "EventProcessable was nullptr!",);

    children->pushBack(object);
}

void EventManager::setPriority(plugin::EventType event_type, uint8_t priority) {
    priorities[(int)(event_type)] = priority;
}

void EventManager::unregisterObject(plugin::EventProcessableI *object) {
    ON_ERROR(!object, "EventProcessable was nullptr!",);

    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i] == object) {
            children->remove(i);
            return;
        }
    }
}

bool EventManager::onMouseMove(plugin::MouseContext context)   {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[(int)(plugin::EventType::MouseMove)]) {
            (*children)[i]->onMouseMove(context);
        }
    }

    return true;
}

bool EventManager::onMouseRelease(plugin::MouseContext context)    {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[(int)(plugin::EventType::MouseRelease)]) {
            (*children)[i]->onMouseRelease(context);
        }
    }

    return true;
}

bool EventManager::onMousePress(plugin::MouseContext context)    {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[(int)(plugin::EventType::MousePress)]) {
            (*children)[i]->onMousePress(context);
        }
    }

    return true;
}

bool EventManager::onKeyboardPress(plugin::KeyboardContext context) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[(int)(plugin::EventType::KeyPress)]) {
            (*children)[i]->onKeyboardPress(context);
        }
    }

    return true;
}

bool EventManager::onKeyboardRelease(plugin::KeyboardContext context) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[(int)(plugin::EventType::KeyRelease)]) {
            (*children)[i]->onKeyboardRelease(context);
        }
    }

    return true;
}

bool EventManager::onClock(uint64_t delta) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[(int)(plugin::EventType::Clock)]) {
            (*children)[i]->onClock(delta);
        }
    }

    return true;
}