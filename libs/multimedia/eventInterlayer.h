#ifndef _EVENTINTERLAYER_h_
#define _EVENTINTERLAYER_h_


#include <SFML/Graphics.hpp>
#include <chrono>

#include "multimedia.h"
#include "../../src/events/events.h"
#include "../../src/ui/renderable.h"

void runEventCycle(RenderTarget& renderTarget, EventManager& eventBoy, Widget& drawWidget);

#endif