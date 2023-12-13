#ifndef _EVENTS_h_
#define _EVENTS_h_

#include "../../libs/multimedia/multimedia.h"
#include "../../src/plugin.h"

using EventType = plugin::EventType;

static const int EVENT_TYPES_NUM = (int)(plugin::EventType::NumOfEvents);

class EventProcessable : public plugin::EventProcessableI {
private:
    uint8_t priority = 0;

public:
    EventProcessable();
    EventProcessable(uint8_t _priority);

    uint8_t getPriority() const override;
};

class EventManager : public EventProcessable, public plugin::EventManagerI {
private:
    List<plugin::EventProcessableI*>* children = nullptr;
    int priorities[EVENT_TYPES_NUM];

public:
    EventManager();

    ~EventManager();

    void registerObject  (plugin::EventProcessableI *object)   override;
    void setPriority     (plugin::EventType, uint8_t priority) override;
    void unregisterObject(plugin::EventProcessableI *object)   override;

    void privatizeEvents(List<EventType>& events, int priority);
    void resetPriorities();

    bool onMouseMove      (plugin::MouseContext context)    override;
    bool onMouseRelease   (plugin::MouseContext context)    override;
    bool onMousePress     (plugin::MouseContext context)    override;
    bool onKeyboardPress  (plugin::KeyboardContext context) override;
    bool onKeyboardRelease(plugin::KeyboardContext context) override;
    bool onClock          (uint64_t delta)                  override;
};

#endif