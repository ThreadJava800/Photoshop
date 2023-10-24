#ifndef _EVENTS_h_
#define _EVENTS_h_

#include "../../libs/multimedia/multimedia.h"

enum EventType {
    KEY_PRESSED,
    KEY_RELEASED,
    
    MOUSE_PRESSED,
    MOUSE_MOVE,
    MOUSE_RELEASED
};

static const int EVENT_TYPES_NUM = MOUSE_RELEASED - KEY_PRESSED + 1;

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

class EventManager : public EventProcessable {
private:
    List<EventProcessable*>* children = nullptr;
    int priorities[EVENT_TYPES_NUM];

public:
    EventManager();

    ~EventManager();

    void registerObject  (EventProcessable* eventProc);
    void unregisterObject(EventProcessable* eventProc);
    void privatizeEvents (List<EventType> events, int priority);
    void resetPriorities ();

    bool onKeyPressed (MKeyboard key) override;
    bool onKeyReleased(MKeyboard key) override;

    bool onMousePressed (MPoint pos, MMouse btn) override;
    bool onMouseReleased(MPoint pos, MMouse btn) override;
    bool onMouseMove    (MPoint pos, MMouse btn) override;
};

#endif