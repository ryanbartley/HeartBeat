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
	
const EventType ApproachEvent::TYPE = Hash::HASHER("ApproachEvent");
const EventType TableEvent::TYPE = Hash::HASHER("TableEvent");
const EventType DepartEvent::TYPE = Hash::HASHER("DepartEvent");
	
}