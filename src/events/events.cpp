#include "events.h"

EventProcessable::EventProcessable() :
    priority(0)         {}

EventProcessable::EventProcessable(uint8_t _priority) :
    priority(_priority) {}

uint8_t EventProcessable::getPriority() {
    return priority;
}

EventManager::EventManager() :
    EventProcessable()  {
        resetPriorities();
        children = new List<EventProcessable*>();
    }

void EventManager::registerObject(EventProcessable* eventProc) {
    ON_ERROR(!eventProc, "EventProcessable was nullptr!",);

    children->pushBack(eventProc);
}

void EventManager::unregisterObject(EventProcessable* eventProc) {
    ON_ERROR(!eventProc, "EventProcessable was nullptr!",);

    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i] == eventProc) {
            children->remove(i);
            return;
        }
    }
}

void EventManager::privatizeEvents(List<EventType> events, int priority) {
    size_t eventCnt = events.getSize();
    for (size_t i = 0; i < eventCnt; i++) {
        priorities[events[i]] = priority;
    }
}

void EventManager::resetPriorities() {
    for (size_t i = 0; i < EVENT_TYPES_NUM; i++)
        priorities[i] = 0;
}

bool EventManager::onKeyPressed(MKeyboard key) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[KEY_PRESSED]) {
            (*children)[i]->onKeyPressed(key);
        }
    }

    return true;
}

bool EventManager::onKeyReleased(MKeyboard key) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[KEY_RELEASED]) {
            (*children)[i]->onKeyReleased(key);
        }
    }

    return true;
}

bool EventManager::onMousePressed(MPoint pos, MMouse btn) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[MOUSE_PRESSED]) {
            (*children)[i]->onMousePressed(pos, btn);
        }
    }

    return true;
}

bool EventManager::onMouseReleased(MPoint pos, MMouse btn) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[MOUSE_RELEASED]) {
            (*children)[i]->onMouseReleased(pos, btn);
        }
    }

    return true;
}

bool EventManager::onMouseMove(MPoint pos, MMouse btn) {
    size_t childCnt = children->getSize();
    for (size_t i = 0; i < childCnt; i++) {
        if ((*children)[i]->getPriority() >= priorities[MOUSE_MOVE]) {
            (*children)[i]->onMouseMove(pos, btn);
        }
    }

    return true;
}