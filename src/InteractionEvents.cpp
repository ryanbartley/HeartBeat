//
//  InteractionEvents.cpp
//  LibUrgTest
//
//  Created by Ryan Bartley on 10/8/14.
//
//

#include "InteractionEvents.h"

#include "EventManager.h"

namespace heartbeat {
	
const EventType ApproachEvent::TYPE = EventManager::HASHER("ApproachEvent");
const EventType TableEvent::TYPE = EventManager::HASHER("TableEvent");
	
}