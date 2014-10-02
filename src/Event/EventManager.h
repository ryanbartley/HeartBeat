//
//  EventManager.h
//  RendererTest
//
//  Created by Ryan Bartley on 8/17/14.
//
//

#pragma once

#include "EventManagerBase.h"
#include "Common.h"
#include <boost/signals2/signal.hpp>
#include <deque>

namespace heartbeat {
	
constexpr auto NUM_QUEUES = 2u;
	
class EventManager : public EventManagerBase {
	using EventListenerList = std::list<EventListenerDelegate>;
	using EventListenerMap	= std::map<EventType, EventListenerList>;
	using EventQueue		= std::deque<EventDataRef>;
	
public:
	
	static EventManagerRef create( const std::string &name, bool setAsGlobal );
	
	virtual ~EventManager();
	
	virtual bool addListener( const EventListenerDelegate &eventDelegate, const EventType &type ) override;
	virtual bool removeListener( const EventListenerDelegate &eventDelegate, const EventType &type ) override;
	
	virtual bool triggerEvent( const EventDataRef &event ) override;
	virtual bool queueEvent( const EventDataRef &event ) override;
	virtual bool abortEvent( const EventType &type, bool allOfType = false ) override;
	
	virtual bool update( uint64_t maxMillis = kINFINITE ) override;

private:
	explicit EventManager( const std::string &name, bool setAsGlobal );
	
	EventListenerMap					mEventListeners;
	std::array<EventQueue, NUM_QUEUES>  mQueues;
	uint32_t							mActiveQueue;
};
	
}