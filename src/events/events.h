#ifndef _EVENTS_h_
#define _EVENTS_h_

#include "../../libs/multimedia/multimedia.h"

static const int EVENT_TYPES_NUM = 5;

enum EventType {
    KEY_PRESSED,
    KEY_RELEASED,
    MOUSE_PRESSED,
    MOUSE_MOVE,
    MOUSE_RELEASED
};

class EventProcessable {
private:
    uint8_t priority = 0;

public:
    EventProcessable();
    EventProcessable(uint8_t _priority);

    uint8_t getPriority();

    virtual bool onKeyPressed (MKeyboard key) = 0;
    virtual bool onKeyReleased(MKeyboard key) = 0;

    virtual bool onMousePressed (MPoint pos, MMouse btn) = 0;
    virtual bool onMouseReleased(MPoint pos, MMouse btn) = 0;
    virtual bool onMouseMove    (MPoint pos, MMouse btn) = 0;
};

class Window;
class EventManager : public EventProcessable {
private:
    Window* mainWindowPtr  = nullptr;
    // Logger* logger;

    List<EventProcessable*>* children = nullptr;
    int priorities[EVENT_TYPES_NUM];

public:
    EventManager();
    EventManager(Window* _mainWindowPtr);

    ~EventManager();

    void registerObject  (EventProcessable* eventProc);
    void unregisterObject(EventProcessable* eventProc);
    void privatizeEvents (List<EventType> events, int priority);
    void resetPriorities ();
};

#endif